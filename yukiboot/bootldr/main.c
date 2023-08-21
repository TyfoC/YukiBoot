#include <stdio.h>

#include <segoff.h>
#include <mbr.h>
#include <sib.h>
#include <panic.h>
#include <pmm.h>
#include <rbfs.h>
#include <fmt-cfg.h>
#include <inl-asm.h>

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

size_t PartitionHeaderAddr;
size_t BootConfigFileHeaderAddress;
RBFSPartitionHeader_t PartitionHeader __attribute__((__aligned__(8)));
RBFSFileHeader_t BootConfigFileHeader __attribute__((__aligned__(8)));
const rbfs_hash_t BootConfigFilePathHash = 0x0025C03B092E8636;			// rbfs_hash_str("boot.cfg")

BootUnit_t* build_boot_unit_table(BootUnit_t* table, size_t* length) {

	BootUnit_t bootUnit;
	for (size_t i = 0; i < MBR_MAX_NUMBER_OF_PARTITIONS; i++) {
		// `invalid` entry
		if (
			MasterBootRecord.PartitionTable[i].Status != 0 &&
			MasterBootRecord.PartitionTable[i].Status != MBR_PARTITION_STATUS_BOOTABLE
		) continue;

		bootUnit = build_boot_unit(&MasterBootRecord.PartitionTable[i]);

		table = (BootUnit_t*)realloc(table, (*length + 1) * sizeof(BootUnit_t));
		if (!table) panic("Error: failed to allocate memory!\r\n");
		
		memcpy(&table[*length], &bootUnit, sizeof(BootUnit_t));
		++(*length);

		// if (!(bootUnit.Type == MBR_PARTITION_TYPE_NEAR_EBR || bootUnit.Type == MBR_PARTITION_TYPE_FAR_EBR)) continue;
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
	DBG_PRINTF("EBDA segment: %#x\r\n", SystemInfoBlock.EBDASegment);

	/*	==================
		CPUID
	====================*/

	SystemInfoBlock.CPUID = (size_t)cpuid_is_present();
	DBG_PRINTF("CPUID: %u\r\n", SystemInfoBlock.CPUID);

	/*	==================
		Drive
	====================*/

	SystemInfoBlock.DriveIndex = (size_t)driveIndex;
	DBG_PRINTF("BIOS drive index: %#x\r\n", SystemInfoBlock.DriveIndex);
	if (!drive_select(SystemInfoBlock.DriveIndex)) panic("Error: failed to select drive (%#x)!\r\n", SystemInfoBlock.DriveIndex);

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

	size_t numOfBootUnits = 0;
	BootUnit_t* bootUnits = build_boot_unit_table(NULL, &numOfBootUnits);

	size_t tmpStrLen, i, j;
	RBFSHeader_t* rbfsHdr;
	SystemInfoBlock.NumberOfBootUnits = 0;
	SystemInfoBlock.RBFSBootUnits = NULL;

	for (i = 0; i < numOfBootUnits; i++) {
		if (bootUnits[i].Type != MBR_PARTITION_TYPE_ALT_OS) continue;

		if (drive_get_index() != bootUnits[i].DriveIndex && !drive_select(bootUnits[i].DriveIndex)) {
			DBG_PRINTF("Warning: failed to select drive (%#x)!\r\n", bootUnits[i].DriveIndex);
			continue;
		}

		//	Check for the existence of an `RBFSHeader_t` structure
		if (!rbfs_init(bootUnits[i].Address)) continue;

		rbfsHdr = rbfs_get_header();

		for (j = 0; j < (size_t)rbfsHdr->NumberOfPartitions; j++) {
			PartitionHeaderAddr = rbfs_get_partition_header(j, &PartitionHeader);
			if (PartitionHeaderAddr == ((size_t)-1)) {
				DBG_PUTS("Warning: failed to read partition header!\r\n");
				DBG_PRINTF(
					"\tBIOS drive index: %#x, partition header index: %u\r\n",
					bootUnits[i].DriveIndex, j
				);

				continue;
			}

			BootConfigFileHeaderAddress = rbfs_get_file_header(
				PartitionHeaderAddr,
				BootConfigFilePathHash,
				&BootConfigFileHeader
			);

			if (BootConfigFileHeaderAddress == ((size_t)-1)) continue;

			SystemInfoBlock.RBFSBootUnits = (RBFSBootUnit_t*)realloc(
				SystemInfoBlock.RBFSBootUnits,
				sizeof(RBFSBootUnit_t) * (SystemInfoBlock.NumberOfBootUnits + 1)
			);

			if (!SystemInfoBlock.RBFSBootUnits) panic("Error: failed to allocate memory!\r\n");

			SystemInfoBlock.RBFSBootUnits[SystemInfoBlock.NumberOfBootUnits].DriveIndex = bootUnits[i].DriveIndex;
			SystemInfoBlock.RBFSBootUnits[SystemInfoBlock.NumberOfBootUnits].BootConfigFileHeaderAddress = BootConfigFileHeaderAddress;

			tmpStrLen = strlen(&PartitionHeader.Name[0]);
			SystemInfoBlock.RBFSBootUnits[SystemInfoBlock.NumberOfBootUnits].PartitionName = (char*)malloc(tmpStrLen + 1);
			if (
				!SystemInfoBlock.RBFSBootUnits[SystemInfoBlock.NumberOfBootUnits].PartitionName
			) panic("Error: failed to allocate memory!\r\n");

			memcpy(
				&SystemInfoBlock.RBFSBootUnits[SystemInfoBlock.NumberOfBootUnits].PartitionName[0],
				&PartitionHeader.Name[0],
				tmpStrLen + 1
			);

			SystemInfoBlock.NumberOfBootUnits += 1;
		}
	}

	tty_set_text_color((TTY_COLOR_LIGHT_BLUE << 4) | TTY_COLOR_WHITE);
	tty_clear_screen();
	tty_set_line(0);
	tty_set_column(0);

	size_t numOfLines = tty_get_number_of_lines();
	size_t numOfColumns = tty_get_number_of_columns();
	size_t lastColIndex = numOfColumns - 1;

	// line #0
	putchar(0xC9);
	for (i = 0; i < numOfColumns - 2; i++) putchar(0xCD);
	putchar(0xBB);

	// line #1
	putchar(0xBA);
	tty_set_column((numOfColumns - 42) >> 1);
	puts("yukiboot bootloader by Noimage PNG (1.0.0)");
	tty_set_column(lastColIndex);
	putchar(0xBA);

	// line #2
	putchar(0xC7);
	for (i = 0; i < numOfColumns - 2; i++) putchar(0xC4);
	putchar(0xB6);

	// line #3 - line #(numOfLines - 4)
	for (i = 3; i < numOfLines - 3; i++) {
		tty_set_column(0);
		putchar(0xBA);
		tty_set_column(lastColIndex);
		putchar(0xBA);
	}

	// line #(numOfLines - 3)
	putchar(0xBA);
	for (i = 0; i < numOfColumns - 2; i++) putchar(0xCD);
	putchar(0xBA);

	uint8_t pressedKey = 0;
	uint8_t curColor = tty_get_text_color();
	size_t curLineIndex = 3, curColIndex;
	size_t firstUnitIndex = 0, selUnitOffset = 0, selUnitIndex = 0;

	// fix partition names
	for (i = 0; i < SystemInfoBlock.NumberOfBootUnits; i++) {
		tmpStrLen = strlen(SystemInfoBlock.RBFSBootUnits[i].PartitionName);
		if (tmpStrLen >= numOfColumns - 7) {
			SystemInfoBlock.RBFSBootUnits[i].PartitionName[numOfColumns - 7] = '\0';
			SystemInfoBlock.RBFSBootUnits[i].PartitionName[numOfColumns - 8] = '.';
			SystemInfoBlock.RBFSBootUnits[i].PartitionName[numOfColumns - 9] = '.';
			SystemInfoBlock.RBFSBootUnits[i].PartitionName[numOfColumns - 10] = '.';
		}
	}

	if (SystemInfoBlock.NumberOfBootUnits) {
		while (true) {
			tty_set_line(3);
			tty_set_column(1);
			tty_set_text_color(curColor);

			for (
				i = firstUnitIndex, j = curLineIndex;
				i < SystemInfoBlock.NumberOfBootUnits && j < numOfLines - 3;
				i++, j++
			) {
				printf("%u) %s", i + 1, SystemInfoBlock.RBFSBootUnits[i].PartitionName);
				
				curColIndex = tty_get_column();
				while (curColIndex <= lastColIndex - 1) {
					putchar(' ');
					++curColIndex;
				}

				putchar(0xBA);
				putchar(0xBA);
			}

			tty_set_line(3 + selUnitOffset);
			tty_set_column(1);
			tty_set_text_color((TTY_COLOR_WHITE << 4) | TTY_COLOR_LIGHT_CYAN);
			printf("%u) %s\r\n", selUnitIndex + 1, SystemInfoBlock.RBFSBootUnits[selUnitIndex].PartitionName);
			tty_set_text_color(curColor);

			while (true) {
				if (inb(0x64) & 1) {
					pressedKey = inb(0x60);
					if (pressedKey == 0xE0) while (inb(0x64) & 1) pressedKey = inb(0x60);
					break;
				}
			}

			// cursor up/keypad 8 or W 
			if (pressedKey == 0x48 || pressedKey == 0x11) {
				if (selUnitOffset) --selUnitIndex;
				else {
					if (firstUnitIndex) {
						--firstUnitIndex;
						--selUnitIndex;
					}
				}

				selUnitOffset = selUnitIndex - firstUnitIndex;
			}

			// cursor down/keypad 2 or S
			else if (pressedKey == 0x50 || pressedKey == 0x1F) {
				if (selUnitIndex + 1 < SystemInfoBlock.NumberOfBootUnits) {
					if (curLineIndex + selUnitOffset < numOfLines - 4) ++selUnitIndex;
					else {
						++firstUnitIndex;
						++selUnitIndex;
					}
				}
				
				selUnitOffset = selUnitIndex - firstUnitIndex;
			}

			// space or enter
			else if (pressedKey == 0x39 || pressedKey == 0x1C) break;
		}
	}

	if (SystemInfoBlock.NumberOfBootUnits) {
		__asm__ __volatile__(
			"jmp ."
			::"a"(0x99887766),
			"b"(selUnitIndex)
		);
	}

	if (
		SystemInfoBlock.RBFSBootUnits
	) free_rbfs_boot_units(SystemInfoBlock.RBFSBootUnits, SystemInfoBlock.NumberOfBootUnits);
	__asm__ __volatile__("cli; hlt");
}