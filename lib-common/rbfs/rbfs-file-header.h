#pragma once
#ifndef RBFS_FILE_HEADER_H
#define RBFS_FILE_HEADER_H

#include "rbfs-date-time.h"

enum RBFSFileAttribute {
	RBFS_FILE_ATTRIBUTE_DIRECTORY = 1,
	RBFS_FILE_ATTRIBUTE_SYSTEM = 2,
	RBFS_FILE_ATTRIBUTE_SYMLINK = 4,
	RBFS_FILE_ATTRIBUTE_ARCHIVE = 8,
	RBFS_FILE_ATTRIBUTE_HIDDEN = 16,
	RBFS_FILE_ATTRIBUTE_READ_ONLY = 32,
	RBFS_FILE_ATTRIBUTE_TEMPORARY = 64
};

/*
	Describes the characteristics of the file
	and points to the first chunk of its data.
*/
typedef struct __attribute__((__packed__)) {
	//			Name in UTF-8 encoding
	/*0x000*/	char			Name[440];

	//			Important for file header search
	/*0x1B8*/	rbfs_hash_t		PathHash;

	//			Important for file header search
	/*0x1C0*/	rbfs_hash_t		ParentPathHash;

	//			RBFS_FILE_ATTRIBUTE_*
	/*0x1C8*/	uint64_t		Attributes;

	//			Not used in this revision(3)
	/*0x1D0*/	RBFSTime_t		CreationTime;
	/*0x1D4*/	RBFSTime_t		LastAccessTime;
	/*0x1D8*/	RBFSTime_t		LastModificationTime;
	
	//			Not used in this revision(3)
	/*0x1DC*/	RBFSDate_t		CreationDate;
	/*0x1E0*/	RBFSDate_t		LastAccessDate;
	/*0x1E4*/	RBFSDate_t		LastModificationDate;
	
	/*
		If it is a directory, then the field is 0;

		If it is a file then this field is equal to the size of the file.
	*/
	/*0x1E8*/	uint64_t		SizeInBytes;

	/*
		If this is a directory, then this field is equal
		to the number of elements inside the directory;

		if this is a file, then the number of sectors
		in which the file data is written.
	*/
	/*0x1F0*/	uint64_t		LengthInSectors;

	//			LBA OFFSET(!)
	/*0x1F8*/	int64_t			FirstChunkOffset;
} RBFSFileHeader_t;

#endif