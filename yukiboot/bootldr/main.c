#include <stdio.h>

#include <segoff.h>
#include <mbr.h>
#include <sib.h>
#include <panic.h>
#include <pmm.h>

#include <fpu.h>
#include <cpuid-support.h>
#include <drive.h>
#include <vesa.h>
#include <mem.h>

#define DEBUG
#include <debug.h>

extern char* __PTR_BASE_ADDR__;
extern char* __PTR_END_ADDR__;

SystemInfoBlock_t SystemInfoBlock __attribute__((__aligned__(8)));
MasterBootRecord_t MasterBootRecord __attribute__((__aligned__(8)));
ExtendedBootRecord_t ExtendedBootRecord __attribute__((__aligned__(8)));

BootUnit_t* build_boot_unit_table(uint8_t driveIndex, BootUnit_t* table, size_t* length, size_t* ebrIndex) {
	BootUnit_t bootUnit;
	MasterBootRecord_t* buffer = *ebrIndex ? &ExtendedBootRecord : &MasterBootRecord;
	for (size_t i = 0; i < MBR_MAX_NUMBER_OF_PARTITIONS; i++) {
		// `invalid` entry
		if (
			buffer->PartitionTable[i].Status != 0 &&
			buffer->PartitionTable[i].Status != MBR_PARTITION_STATUS_BOOTABLE
		) continue;

		bootUnit.DriveIndex = driveIndex;
		bootUnit.Type = buffer->PartitionTable[i].Type;

		if (
			buffer->PartitionTable[i].StartHead < 0xFE &&
			buffer->PartitionTable[i].StartSectorCylinder != 0xFFFF &&
			!buffer->PartitionTable[i].StartAddress
		) {
			bootUnit.Address = drive_chs_to_lba(
				EXTRACT_CYL_FROM_CYL_SECT(buffer->PartitionTable[i].StartSectorCylinder),
				buffer->PartitionTable[i].StartHead,
				EXTRACT_SECT_FROM_CYL_SECT(buffer->PartitionTable[i].StartSectorCylinder)
			);
		}
		else bootUnit.Address = buffer->PartitionTable[i].StartAddress;

		if (
			buffer->PartitionTable[i].NumberOfSectors
		) bootUnit.NumberOfSectors = buffer->PartitionTable[i].NumberOfSectors;
		else {
			bootUnit.NumberOfSectors = (
				drive_chs_to_lba(
					EXTRACT_CYL_FROM_CYL_SECT(buffer->PartitionTable[i].LastSectorCylinder),
					buffer->PartitionTable[i].LastHead,
					EXTRACT_SECT_FROM_CYL_SECT(buffer->PartitionTable[i].LastSectorCylinder)
				) + 1 - bootUnit.Address
			);
		}

		bootUnit.MBRPartitionEntryIndex = i;
		bootUnit.EBRIndex = *ebrIndex;

		table = (BootUnit_t*)realloc(table, (*length + 1) * sizeof(BootUnit_t));
		if (!table) panic("Error: failed to allocate memory!\r\n");
		
		memcpy(&table[*length], &bootUnit, sizeof(BootUnit_t));
		++(*length);

		if (bootUnit.Type == MBR_PARTITION_TYPE_NEAR_EBR || bootUnit.Type == MBR_PARTITION_TYPE_FAR_EBR) {
			table = build_boot_unit_table(driveIndex, table, length, ebrIndex);
		}
	}

	return table;
}

extern void bootldr_main(unsigned char driveIndex) {
	tty_init(0xB8000, TTY_COLOR_LIGHT_GRAY, 80, 25, 160);

	/*	==================
		FPU
	====================*/

	SystemInfoBlock.FPU = (size_t)fpu_init();
	if (SystemInfoBlock.FPU) puts("Warning: failed to init FPU!\r\n");

	/*	==================
		EBDA segment
	====================*/

	SystemInfoBlock.EBDASegment = (size_t)*(uint16_t*)0x40E;
	DBG_PRINTF("EBDA segment: 0x%x\r\n", SystemInfoBlock.EBDASegment);

	/*	==================
		CPUID
	====================*/

	SystemInfoBlock.CPUID = (size_t)cpuid_is_present();
	DBG_PRINTF("CPUID: %u\r\n", SystemInfoBlock.CPUID);

	/*	==================
		Drive
	====================*/

	SystemInfoBlock.DriveIndex = (size_t)driveIndex;
	DBG_PRINTF("BIOS drive index: 0x%x\r\n", SystemInfoBlock.DriveIndex);
	if (!drive_select(SystemInfoBlock.DriveIndex)) panic("Error: failed to select drive (0x%x)!\r\n", SystemInfoBlock.DriveIndex);

	SystemInfoBlock.NumberOfHDDs = (size_t)drive_get_number_of_hdds();
	DBG_PRINTF("Number of hard disk drives: %u\r\n", SystemInfoBlock.NumberOfHDDs);

	SystemInfoBlock.DriveInterfaceSupportBitmask = (size_t)drive_get_interface_support_bitmask();
	if (SystemInfoBlock.DriveInterfaceSupportBitmask) {
		DBG_PUTS("Interface support bitmask: ");
		if (SystemInfoBlock.DriveInterfaceSupportBitmask & DRIVE_IF_SUPPORT_DAP) DBG_PUTS("DAP, ");
		if (SystemInfoBlock.DriveInterfaceSupportBitmask & DRIVE_IF_SUPPORT_LOCKING_AND_EJECTING) DBG_PUTS("LCK&EJCT, ");
		if (SystemInfoBlock.DriveInterfaceSupportBitmask & DRIVE_IF_SUPPORT_EDD) DBG_PUTS("EDD, ");
		DBG_PUTS("\b\b  \r\n");
	}

	/*	==================
		Memory map
	====================*/

	SystemInfoBlock.MemoryMapPtr = (AddressRangeDescriptor_t*)((size_t)&__PTR_END_ADDR__);
	SystemInfoBlock.NumberOfMemoryRegions = mem_get_map(SystemInfoBlock.MemoryMapPtr);

	/*	==================
		PMM
	====================*/

	const AddressRangeDescriptor_t extraMemoryRegions[] = {
		{ 0, 0x1000, ARD_TYPE_RESERVED, ARD_ATTRIBUTE_PRESENT },			// IVT & BDA & bootloader stack
		{ 0x1000, 0x600, ARD_TYPE_RESERVED, ARD_ATTRIBUTE_PRESENT },		// real mode service loader
		{
			((size_t)&__PTR_BASE_ADDR__), (((size_t)&__PTR_END_ADDR__) - ((size_t)&__PTR_BASE_ADDR__)),
			ARD_TYPE_RESERVED, ARD_ATTRIBUTE_PRESENT
		},																	// bootloader
		{ 0x80000, 0x80000, ARD_TYPE_RESERVED, ARD_ATTRIBUTE_PRESENT },		// EBDA & other
		{ 0xF00000, 0x100000, ARD_TYPE_RESERVED, ARD_ATTRIBUTE_PRESENT }	// ISA hole
	};

	SystemInfoBlock.NumberOfMemoryRegions = pmm_exclude_regions(
		SystemInfoBlock.MemoryMapPtr, SystemInfoBlock.NumberOfMemoryRegions,
		extraMemoryRegions, sizeof(extraMemoryRegions) / sizeof(extraMemoryRegions[0])
	);

	pmm_init(SystemInfoBlock.MemoryMapPtr, SystemInfoBlock.NumberOfMemoryRegions);

	SystemInfoBlock.MemoryMapPtr = pmm_get_regions();
	SystemInfoBlock.NumberOfMemoryRegions = pmm_get_number_of_regions();

	DBG_PRINTF("Memory map address: %#x\r\n", SystemInfoBlock.MemoryMapPtr);
	DBG_PRINTF("Number of memory regions: %u\r\n", SystemInfoBlock.NumberOfMemoryRegions);

	/*	==================
		VESA
	====================*/

	if (!vesa_init()) puts("Warning: failed to init VESA!\r\n");
	else {
		memcpy(&SystemInfoBlock.VESAInfo, vesa_get_info(), sizeof(VESAInfo_t));
		
		DBG_PUTS("VESA:\r\n");

		if (SystemInfoBlock.VESAInfo.Signature != VESA_SIGNATURE) DBG_PUTS("\tWarning: wrong signature!\r\n");

		DBG_PUTS("\tSignature: `");
		DBG_PUTCHAR(SystemInfoBlock.VESAInfo.Signature & 0xFF);
		DBG_PUTCHAR((SystemInfoBlock.VESAInfo.Signature >> 8) & 0xFF);
		DBG_PUTCHAR((SystemInfoBlock.VESAInfo.Signature >> 16) & 0xFF);
		DBG_PUTCHAR((SystemInfoBlock.VESAInfo.Signature >> 24) & 0xFF);

		DBG_PRINTF("`\r\n\tVersion: %u.%u\r\n", SystemInfoBlock.VESAInfo.Version >> 8, SystemInfoBlock.VESAInfo.Version & 0xFF);

		DBG_PRINTF(
			"\tOEM string (%#x): `%s`\r\n",
			SEGOFF_TO_PHYS(SystemInfoBlock.VESAInfo.OEMStringPtr[1], SystemInfoBlock.VESAInfo.OEMStringPtr[0]),
			(char*)SEGOFF_TO_PHYS(SystemInfoBlock.VESAInfo.OEMStringPtr[1], SystemInfoBlock.VESAInfo.OEMStringPtr[0])
		);

		DBG_PUTS("\tCapabilities:\r\n");
		if (
			SystemInfoBlock.VESAInfo.Capabilities & VESA_CAPABILITIES_DAC_WIDTH_SWITCHABLE
		) DBG_PUTS("\t+\tDAC width is switchable to 8 bits per primary color\r\n");
		else DBG_PUTS("\t-\tDAC is fixed width, with 6 bits per primary color\r\n");
		if (
			SystemInfoBlock.VESAInfo.Capabilities & VESA_CAPABILITIES_CTRL_IS_NOT_VGA_COMPATIBLE
		) DBG_PUTS("\t+\tController is not VGA compatible\r\n");
		else DBG_PUTS("\t-\tController is VGA compatible\r\n");
		if (
			SystemInfoBlock.VESAInfo.Capabilities & VESA_CAPABILITIES_PROG_RAMDAC_BLANK_BIT
		) DBG_PUTS("\t+\tWhen programming large blocks of information to the RAMDAC, use the blank bit in Function 09h\r\n");
		else DBG_PUTS("\t-\tNormal RAMDAC operation\r\n");

		DBG_PRINTF("\tTotal memory (in 64KB blocks): %#x\r\n", SystemInfoBlock.VESAInfo.TotalMemorySize);
	}

	/*	==================
		MBR
	====================*/


	if (drive_read_sectors_ext(0, 1, (void*)&MasterBootRecord) != 1) panic("Error: failed to read MBR!\r\n");

	SystemInfoBlock.NumberOfBootUnits = 0;
	size_t tmpEbrIndex = 0;
	SystemInfoBlock.BootUnits = build_boot_unit_table(
		SystemInfoBlock.DriveIndex, SystemInfoBlock.BootUnits,
		&SystemInfoBlock.NumberOfBootUnits, &tmpEbrIndex
	);

	puts("Boot units:\r\n");
	for (size_t i = 0; i < SystemInfoBlock.NumberOfBootUnits; i++) {
		printf(
			"%u) Drive: %#x, Type: %#x, Address: %#x, Length: %#x\r\n",
			i,
			SystemInfoBlock.BootUnits[i].DriveIndex,
			SystemInfoBlock.BootUnits[i].Type,
			SystemInfoBlock.BootUnits[i].Address,
			SystemInfoBlock.BootUnits[i].NumberOfSectors
		);
	}

	__asm__ __volatile__("cli; hlt");
}