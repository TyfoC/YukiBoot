#include <stdio.h>

#include <segoff.h>
#include <sib.h>
#include <panic.h>

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
char SectorBuffer[DRIVE_SECTOR_SIZE] __attribute__((__aligned__(8)));

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

	SystemInfoBlock.EBDASegment = *(uint16_t*)0x40E;
	DBG_PRINTF("EBDA segment: 0x%x\r\n", SystemInfoBlock.EBDASegment);

	/*	==================
		CPUID
	====================*/

	SystemInfoBlock.CPUID = cpuid_is_present();
	DBG_PRINTF("CPUID: %u\r\n", SystemInfoBlock.CPUID);

	/*	==================
		Drive
	====================*/

	SystemInfoBlock.DriveIndex = driveIndex;
	DBG_PRINTF("BIOS drive index: 0x%x\r\n", SystemInfoBlock.DriveIndex);
	if (!drive_select(SystemInfoBlock.DriveIndex)) panic("Error: failed to select drive (0x%x)!\r\n", SystemInfoBlock.DriveIndex);

	SystemInfoBlock.NumberOfHDDs = drive_get_number_of_hdds();
	DBG_PRINTF("Number of hard disk drives: %u\r\n", SystemInfoBlock.NumberOfHDDs);

	SystemInfoBlock.DriveInterfaceSupportBitmask = drive_get_interface_support_bitmask();
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

	if (drive_read_sectors_ext(0, 1, (void*)&SectorBuffer[0]) != 1) panic("Error: failed to read MBR!\r\n");

	__asm__ __volatile__("cli; hlt");
}