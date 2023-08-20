#pragma once
#ifndef SIB_H
#define SIB_H											// system info block

#include <stdint.h>
#include "vesa.h"
#include "mem.h"

typedef struct __attribute__((__packed__)) {
	uint32_t					DriveIndex;						// BIOS drive index
	uint32_t					NumberOfHDDs;					// number of hard disk drives
	uint16_t					DriveInterfaceSupportBitmask;

	size_t						FPU;							// 1 if FPU is present, otherwise 0

	uint16_t					EBDASegment;					// 0x(0-F)000 (extended BIOS data area segment)

	VESAInfo_t					VESAInfo;						// VESA info

	AddressRangeDescriptor_t*	MemoryMapPtr;					// E820 memory map
	size_t						NumberOfMemoryRegions;
} SystemInfoBlock_t;

#endif