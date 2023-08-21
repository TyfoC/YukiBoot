#pragma once
#ifndef RBFS_PARTITION_HEADER_H
#define RBFS_PARTITION_HEADER_H

#include "rbfs-typedefs.h"

enum RBFSPartitionType {
	RBFS_PARTITION_TYPE_STORAGE,
	RBFS_PARTITION_TYPE_BOOTABLE,
	RBFS_PARTITION_TYPE_SWAP,
	RBFS_PARTITION_TYPE_RAW
};

enum RBFSPartitionAttribute {
	RBFS_PARTITION_ATTRIBUTE_HIDDEN = 1,
	RBFS_PARTITION_ATTRIBUTE_READ_ONLY = 2,
	RBFS_PARTITION_ATTRIBUTE_TEMPORARY = 4,
	RBFS_PARTITION_ATTRIBUTE_UNMOUNTED = 8
};

/*
	Describes the characteristics of a partition and points to the data in that partition.
*/
typedef struct __attribute__((__packed__)) {
	//			Name in UTF-8 encoding
	/*0x000*/	char			Name[460];

	/*0x1CС*/	rbfs_hash_t		NameHash;
	
	//			RBFS_PARTITION_TYPE_*
	/*0x1D4*/	uint32_t		Type;
	
	//			RBFS_PARTITION_ATTRIBUTE_*
	/*0x1D8*/	uint64_t		Attributes;
	
	//			LBA OFFSET(!)
	/*0x1E0*/	int64_t			DataOffset;
	/*0x1E8*/	uint64_t		NumberOfUsedSectors;
	/*0x1F0*/	uint64_t		NumberOfSectors;
	/*0x1F8*/	uint64_t		NumberOfFileHeaders;
} RBFSPartitionHeader_t;

#endif