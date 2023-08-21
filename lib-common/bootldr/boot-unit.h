#pragma once
#ifndef BOOT_UNIT_H
#define BOOT_UNIT_H

#include <stdint.h>
#include "mbr.h"
#include "drive.h"

typedef struct __attribute__((__packed__)) {
	uint8_t			DriveIndex;					// BIOS drive index
	uint8_t			Type;
	uint32_t		Address;					// LBA
	uint32_t		NumberOfSectors;
} BootUnit_t;

BootUnit_t BuildBootUnit(const MBRPartitionEntry_t* partitionTableEntry);

#endif