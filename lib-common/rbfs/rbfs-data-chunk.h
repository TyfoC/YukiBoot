#pragma once
#ifndef RBFS_DATA_CHUNK_H
#define RBFS_DATA_CHUNK_H

#include "rbfs-typedefs.h"

#define RBFS_DATA_CHUNK_DATA_SIZE			504

/*
	Contains the data part of the file and points to the next part
	if the `NextChunkOffset` field is not equal to 0.
*/
typedef struct __attribute__((__packed__)) {
	/*0x000*/	uint8_t	Data[RBFS_DATA_CHUNK_DATA_SIZE];

	//			The field is 0 if this is the last chunk of data
	/*0x1F8*/	int64_t	NextOffset;
} RBFSDataChunk_t;

#endif