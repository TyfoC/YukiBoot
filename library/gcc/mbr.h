#pragma once
#ifndef MBR_H
#define MBR_H

#include <stdint.h>

#define MBR_SIGNATURE								0xAA55

#define MBR_MAX_NUMBER_OF_PARTITIONS				4

#define MBR_PARTITION_STATUS_BOOTABLE				0x80		// active

#define MBR_PARTITION_TYPE_NEAR_EBR					0x05		// extended boot record (below CHS limit)
#define MBR_PARTITION_TYPE_FAR_EBR					0x0F		// extended boot record (above CHS limit)
#define MBR_PARTITION_TYPE_ALT_OS					0x7F

typedef struct __attribute__((__packed__)) {
	uint8_t		Status;
	uint8_t		StartHead;						// start head index
	uint16_t	StartSectorCylinder;			// start sector number[0-5] && cylinder index[8-15][6-7]
	uint8_t		Type;							// MBR_PARTITION_TYPE_*
	uint8_t		LastHead;						// end head index
	uint16_t	LastSectorCylinder;				// end sector number[0-5] && cylinder index[8-15][6-7]
	uint32_t	StartAddress;					// LBA
	uint32_t	NumberOfSectors;
} MBRPartitionEntry_t, EBRPartitionEntry_t;

typedef struct __attribute__((__packed__)) {
	uint8_t				BootstrapCode[446];
	MBRPartitionEntry_t	PartitionTable[4];
	uint16_t			Signature;				// MBR_SIGNATURE
} MasterBootRecord_t, ExtendedBootRecord_t;

#endif