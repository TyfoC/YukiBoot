#pragma once
#ifndef RBFS_H
#define RBFS_H

#include <drive.h>
#include <rbfs-header.h>
#include <rbfs-partition-header.h>
#include <rbfs-file-header.h>
#include <rbfs-data-chunk.h>
#include <rbfs-date-time.h>
#include <rbfs-utils.h>

uint8_t rbfs_init(size_t headerAddress);
RBFSHeader_t* rbfs_get_header(void);
size_t rbfs_get_partition_header(size_t index, RBFSPartitionHeader_t* partitionHeader);
size_t rbfs_get_file_header(size_t partitionAddress, rbfs_hash_t filePathHash, RBFSFileHeader_t* fileHeader);
size_t rbfs_read_file(size_t fileHeaderAddress, void* buffer);

size_t rbfs_find_partition_header(rbfs_hash_t partitionNameHash, RBFSPartitionHeader_t* partitionHeader);

#endif