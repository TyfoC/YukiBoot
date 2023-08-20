#include "mem.h"
#include "stdio.h"

//	Returns the number of detected regions
size_t mem_get_map(void* buffer) {
	size_t resultCnt = 0;
	uint32_t rmStatus = 1, tmpBufferAddr;

	uint32_t entryOffset = 0;
	do {
		CALL_RM_SERVICE(RM_SERVICE_MEM_GET_MAP_ENTRY, :"=a"(rmStatus), "=b"(tmpBufferAddr), "=c"(entryOffset):"b"(entryOffset));
		if (!rmStatus) break;

		memcpy(buffer, (void*)tmpBufferAddr, sizeof(AddressRangeDescriptor_t));
		buffer = (void*)((size_t)buffer + sizeof(AddressRangeDescriptor_t));
		++resultCnt;
	} while (rmStatus && entryOffset);

	return resultCnt;
}