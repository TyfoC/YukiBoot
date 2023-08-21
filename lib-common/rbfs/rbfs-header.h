#pragma once
#ifndef RBFS_HEADER_H
#define RBFS_HEADER_H

#define RBFS_HEADER_SIGNATURE			0x53464252			// 'RBFS'

#include "rbfs-typedefs.h"

/*
	Describes the RBFS version, disk characteristics,
	and also points to an array of structures `RBFSPartitionHeader_t` and usable sectors.
*/
typedef struct __attribute__((__packed__)) {
	//			RBFS_HEADER_SIGNATURE
	/*0x00*/	uint32_t	Signature;
	/*0x04*/	uint32_t	Version;
	/*0x08*/	uint32_t	Checksum;

	//			Field is 0 before initialization
	/*0x0C*/	uint8_t		DiskGUID[16];
	
	//			LBA OFFSET(!)
	/*0x1C*/	int64_t		PartitionTableOffset;
	/*0x24*/	uint64_t	NumberOfPartitions;
	/*0x2C*/	uint64_t	MaximumNumberOfPartitions;

	/*0x34*/	uint32_t	HeaderSize;
	
	/*0x38*/	uint8_t		Reserved[456];
} RBFSHeader_t;

uint8_t rbfs_header_get_checksum(const RBFSHeader_t* header);
uint8_t rbfs_validate_header(const RBFSHeader_t* header);

#endif