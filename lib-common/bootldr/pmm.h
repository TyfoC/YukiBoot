#pragma once
#ifndef PMM_H
#define PMM_H

#include "mem.h"

#define PMM_ALIGN_UP($val, $alignment)			((($val) + ($alignment) - 1) & ~(($alignment) - 1))
#define PMM_ALIGN_DOWN($val, $alignment)		(($val) & ~(($alignment) - 1))

#define PMM_MEM_FRAME_SIZE						0x1000

#define PMM_MEM_FRAME_ATTRIBUTE_BUSY			0x001

#define PMM_OVL_TYPE_NONE						0
#define PMM_OVL_TYPE_BEG						1
#define PMM_OVL_TYPE_END						2
#define PMM_OVL_TYPE_INSIDE						3
#define PMM_OVL_TYPE_OUTSIDE					4

typedef struct __attribute__((__packed__)) {
	uint32_t	Attributes			:12;		// PMM_MEM_FRAME_ATTRIBUTE_*
	uint32_t	FrameIndex			:20;
	size_t		NumberOfBusyFrames;
} MemoryFrameDescriptor_t;

void pmm_init(AddressRangeDescriptor_t* ards, size_t count);
size_t pmm_get_number_of_frame_descriptors(uint64_t regionSize);
size_t pmm_get_ovl_type(const AddressRangeDescriptor_t* region, const AddressRangeDescriptor_t* ovl);
size_t pmm_exclude_regions(AddressRangeDescriptor_t* tgt, size_t tgtLen, const AddressRangeDescriptor_t* extra, size_t extraLen);

AddressRangeDescriptor_t* pmm_get_regions(void);
size_t pmm_get_number_of_regions(void);

void* pmm_malloc(size_t size);
void* pmm_realloc(void* oldMem, size_t size);
size_t pmm_free(void* oldMem);

#endif