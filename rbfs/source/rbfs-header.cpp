#include <rbfs-header.h>

uint8_t rbfs_header_get_checksum(const RBFSHeader_t* header) {
	uint8_t value = 0;
	uint8_t* ptr = (uint8_t*)header;
	for (uint32_t i = 0; i < header->HeaderSize; i++) value += ptr[i];
	return value;
}

uint8_t rbfs_validate_header(const RBFSHeader_t* header) {
	return (
		header->Signature == RBFS_HEADER_SIGNATURE &&
		header->NumberOfPartitions <= header->MaximumNumberOfPartitions &&
		rbfs_header_get_checksum(header) == 0
	);
}