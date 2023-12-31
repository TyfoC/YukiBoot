#pragma once
#ifndef SIB_H
#define SIB_H											// system info block

#include <stdint.h>
#include "vesa.h"
#include "mem.h"
#include "boot-unit.h"

typedef struct /*__attribute__((__packed__))*/ {
	size_t						FPU;							// 1 if FPU is present, otherwise 0

	size_t						EBDASegment;					// 0x(0-F)000 (extended BIOS data area segment)

	size_t						CPUID;							// 1 if CPUID is supported, otherwise 0

	size_t						DriveIndex;						// BIOS drive index
	size_t						NumberOfHDDs;					// number of hard disk drives
	size_t						DriveInterfaceSupportBitmask;

	AddressRangeDescriptor_t*	MemoryMapPtr;					// E820 memory map
	size_t						NumberOfMemoryRegions;

	VESAInfo_t					VESAInfo;						// VESA info

	RBFSBootUnit_t*				RBFSBootUnits;
	size_t						NumberOfBootUnits;

	size_t						VideoBufferAddress;
	size_t						VideoModeWidth;
	size_t						VideoModeHeight;
	size_t						VideoModeBitsPerPixel;
	size_t						VideoModeBytesPerScanLine;
} SystemInfoBlock_t;

#endif