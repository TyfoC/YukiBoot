#include "rbfs.h"

static size_t header_address_ = 0;
static RBFSHeader_t header_ __attribute__((__aligned__(8)));
static RBFSPartitionHeader_t tmp_partition_header_ __attribute__((__aligned__(8)));
static RBFSFileHeader_t tmp_file_header_ __attribute__((__aligned__(8)));

//	headerAddress - LBA of RBFS header
uint8_t rbfs_init(size_t headerAddress) {
	if (drive_read_sectors_ext(headerAddress, 1, &header_) != 1) return 0;
	if (!rbfs_validate_header(&header_)) return 0;

	header_address_ = headerAddress;
	return 1;
}

RBFSHeader_t* rbfs_get_header(void) {
	return &header_;
}

//	Returns address of partition header or (-1)
size_t rbfs_get_partition_header(size_t index, RBFSPartitionHeader_t* partitionHeader) {
	if (index >= (size_t)header_.MaximumNumberOfPartitions) return (long unsigned int)-1;

	if (!partitionHeader) partitionHeader = &tmp_partition_header_;

	size_t partitionHeaderAddr = (size_t)((int64_t)header_address_ + header_.PartitionTableOffset) + index;
	return drive_read_sectors_ext(
		(uint64_t)partitionHeaderAddr, 1, partitionHeader
	) == 1 ? partitionHeaderAddr : (long unsigned int)-1;
}

//	Returns address of file header or (-1)
size_t rbfs_get_file_header(size_t partitionHeaderAddress, rbfs_hash_t filePathHash, RBFSFileHeader_t* fileHeader) {
	if (drive_read_sectors_ext((uint64_t)partitionHeaderAddress, 1, &tmp_partition_header_) != 1) return (long unsigned int)-1;

	if (!fileHeader) fileHeader = &tmp_file_header_;

	size_t fileHdrAddr = (size_t)((int64_t)partitionHeaderAddress + tmp_partition_header_.DataOffset);


	for (size_t i = 0; i < (size_t)tmp_partition_header_.NumberOfFileHeaders; i++) {
		if (drive_read_sectors_ext((uint64_t)fileHdrAddr, 1, fileHeader) != 1) return (long unsigned int)-1;

		if (fileHeader->PathHash == filePathHash) return fileHdrAddr;
		++fileHdrAddr;
	}

	return (long unsigned int)-1;
}

//	Returns number of bytes read
size_t rbfs_read_file(size_t fileHeaderAddress, void* buffer) {
	if (drive_read_sectors_ext((uint64_t)fileHeaderAddress, 1, &tmp_file_header_) != 1) return 0;

	size_t resultCnt = 0;
	size_t dataChunkAddr = fileHeaderAddress + tmp_file_header_.FirstChunkOffset;
	for (size_t i = 0; i < (size_t)tmp_file_header_.LengthInSectors; i++) {
		if (drive_read_sectors_ext((uint64_t)dataChunkAddr, 1, buffer) != 1) return resultCnt;

		resultCnt += RBFS_DATA_CHUNK_DATA_SIZE;
		buffer = (void*)((size_t)buffer + RBFS_DATA_CHUNK_DATA_SIZE);
		dataChunkAddr = (size_t)((int64_t)dataChunkAddr + ((RBFSDataChunk_t*)buffer)->NextOffset);
		if (!dataChunkAddr) break;
	}

	return (size_t)tmp_file_header_.SizeInBytes;
}

//	Returns address of partition header or (-1)
size_t rbfs_find_partition_header(rbfs_hash_t partitionNameHash, RBFSPartitionHeader_t* partitionHeader) {
	if (!partitionHeader) partitionHeader = &tmp_partition_header_;
	size_t partitionHeaderAddr = (size_t)((int64_t)header_address_ + header_.PartitionTableOffset);

	for (size_t i = 0; i < (size_t)header_.NumberOfPartitions; i++) {
		if (drive_read_sectors_ext((uint64_t)partitionHeaderAddr, 1, partitionHeader) != 1) return (long unsigned int)-1;

		if (partitionHeader->NameHash == partitionNameHash) return partitionHeaderAddr;

		++partitionHeaderAddr;
	}

	 return (long unsigned int)-1;
}