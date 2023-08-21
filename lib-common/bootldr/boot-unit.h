#pragma once
#ifndef BOOT_UNIT_H
#define BOOT_UNIT_H

#include <stdint.h>
#include <stdlib.h>
#include "mbr.h"
#include "drive.h"

typedef struct __attribute__((__packed__)) {
	uint8_t			DriveIndex;						// BIOS drive index
	uint8_t			Type;
	uint32_t		Address;						// LBA
	uint32_t		NumberOfSectors;
} BootUnit_t;

typedef struct __attribute__((__packed__)) {
	uint8_t			DriveIndex;						// BIOS drive index
	size_t			PartitionHeaderAddress;			// LBA
	size_t			CfgFileHeaderAddress;			// LBA
	char*			PartitionName;
} RBFSBootUnit_t;

BootUnit_t build_boot_unit(const MBRPartitionEntry_t* partitionTableEntry);
void free_rbfs_boot_units(RBFSBootUnit_t* units, size_t count);

#endif