#include "pmm.h"

static AddressRangeDescriptor_t* regions_ = 0;
static size_t number_of_regions_ = 0;

void pmm_init(AddressRangeDescriptor_t* ards, size_t count) {
	regions_ = ards;

	AddressRangeDescriptor_t extraDescriptor = {
		PMM_ALIGN_UP((size_t)ards, PMM_MEM_FRAME_SIZE),
		PMM_ALIGN_UP((size_t)ards + (count + 2) * sizeof(AddressRangeDescriptor_t), PMM_MEM_FRAME_SIZE) - (size_t)ards,
		ARD_TYPE_RESERVED, ARD_ATTRIBUTE_PRESENT
	};

	count = pmm_exclude_regions(ards, count, &extraDescriptor, 1);

	for (size_t i = 0; i < count; i++) {
		ards[i].Address = PMM_ALIGN_UP(ards[i].Address, PMM_MEM_FRAME_SIZE);
		ards[i].Size = PMM_ALIGN_DOWN(ards[i].Size, PMM_MEM_FRAME_SIZE);
	}

	for (size_t i = 0; i < count; i++) {
		if (
			!pmm_get_number_of_frame_descriptors(ards[i].Size) ||
			ards[i].Address >= 0x100000000 ||
			ards[i].Address + ards[i].Size > 0x100000000
		) {
			memcpy(&ards[i], &ards[i + 1], (count - i - 1) * sizeof(AddressRangeDescriptor_t));
			count -= 1;
			continue;
		}
	}

	number_of_regions_ = count;

	size_t framesCount, j;
	MemoryFrameDescriptor_t* frameDescriptor;
	for (size_t i = 0; i < count; i++) {
		if (regions_[i].Type == ARD_TYPE_USABLE) {
			frameDescriptor = (MemoryFrameDescriptor_t*)(size_t)regions_[i].Address;
			framesCount = pmm_get_number_of_frame_descriptors(regions_[i].Size);

			for (j = 0; j < framesCount; j++) {
				frameDescriptor->Attributes = 0;
				frameDescriptor->FrameIndex = j;
				frameDescriptor->NumberOfBusyFrames = 0;
				++frameDescriptor;
			}
		}
	}
}

size_t pmm_get_number_of_frame_descriptors(uint64_t regionSize) {
	if (regionSize < PMM_ALIGN_UP(PMM_MEM_FRAME_SIZE + sizeof(MemoryFrameDescriptor_t), PMM_MEM_FRAME_SIZE)) return 0;
	return PMM_ALIGN_DOWN(regionSize, PMM_MEM_FRAME_SIZE) / (PMM_MEM_FRAME_SIZE + sizeof(MemoryFrameDescriptor_t)) - 1;
}

size_t pmm_get_ovl_type(const AddressRangeDescriptor_t* region, const AddressRangeDescriptor_t* ovl) {
	uint64_t overlapEnd = (uint64_t)(ovl->Address + ovl->Size);
	uint64_t regionEnd = (uint64_t)(region->Address + region->Size);

	if (ovl->Address <= region->Address && overlapEnd > (uint64_t)region->Address && overlapEnd < regionEnd) return PMM_OVL_TYPE_BEG;
	else if ((uint64_t)ovl->Address < regionEnd && ovl->Address > region->Address && overlapEnd >= regionEnd) return PMM_OVL_TYPE_END;
	else if (ovl->Address >= region->Address && overlapEnd <= regionEnd) return PMM_OVL_TYPE_INSIDE;
	else if (ovl->Address <= region->Address && overlapEnd >= regionEnd) return PMM_OVL_TYPE_OUTSIDE;

	return PMM_OVL_TYPE_NONE;
}

//	Returns number of regions
size_t pmm_exclude_regions(AddressRangeDescriptor_t* tgt, size_t tgtLen, const AddressRangeDescriptor_t* extra, size_t extraLen) {
	uint64_t myStart, myLen, myEnd, entryStart, entryLen, entryEnd;
	uint8_t overlapType;

	for (size_t i = 0; i < tgtLen; i++) {
		if (tgt[i].Type == ARD_TYPE_USABLE) {
			for (size_t j = 0; j < extraLen; j++) {
				if (extra[j].Size) {
					overlapType = pmm_get_ovl_type(&tgt[i], &extra[j]);

					myStart = tgt[i].Address;
					myLen = tgt[i].Size;
					myEnd = myStart + myLen;

					entryStart = extra[j].Address;
					entryLen = extra[j].Size;
					entryEnd = entryStart + entryLen;

					if (overlapType == PMM_OVL_TYPE_BEG) {
						tgt[i].Size -= entryEnd - myStart;
						tgt[i].Address = entryEnd;
					}
					else if (overlapType == PMM_OVL_TYPE_END) {
						tgt[i].Size = entryStart - myStart;
					}
					else if (overlapType == PMM_OVL_TYPE_INSIDE) {
						for (size_t k = tgtLen; k > i; k--) tgt[k] = tgt[k - 1];
						tgt[i + 1].Address = entryEnd;
						tgt[i + 1].Size = myEnd - entryEnd;
						tgt[i + 1].Type = tgt[i].Type;
						tgt[i].Size = entryStart - myStart;

						tgtLen += 1;
					}
					else if (overlapType == PMM_OVL_TYPE_OUTSIDE) {
						tgt[i].Type = ARD_TYPE_RESERVED;
					}
				}
			}
		}
	}

	return tgtLen;
}

AddressRangeDescriptor_t* pmm_get_regions(void) {
	return regions_;
}

size_t pmm_get_number_of_regions(void) {
	return number_of_regions_;
}

void* pmm_malloc(size_t size) {
	MemoryFrameDescriptor_t* frameDescriptors;
	size_t framesCount, j, k, framesAddr;
	uint8_t framesFree;
	
	size_t count = PMM_ALIGN_UP(size, PMM_MEM_FRAME_SIZE) / PMM_MEM_FRAME_SIZE;

	for (size_t i = 0; i < number_of_regions_; i++) {
		if (regions_[i].Type == ARD_TYPE_USABLE) {
			frameDescriptors = (MemoryFrameDescriptor_t*)(size_t)regions_[i].Address;
			framesCount = pmm_get_number_of_frame_descriptors(regions_[i].Size);
			framesAddr = PMM_ALIGN_UP((size_t)regions_[i].Address + sizeof(MemoryFrameDescriptor_t) * framesCount, PMM_MEM_FRAME_SIZE);

			if (count <= framesCount) {
				for (j = 0; j + count <= framesCount; j++) {
					framesFree = 1;

					for (k = j; k < j + count; k++) {
						if (frameDescriptors[k].Attributes & PMM_MEM_FRAME_ATTRIBUTE_BUSY) {
							framesFree = 0;
							break;
						}
					}

					if (framesFree) {
						frameDescriptors[j].NumberOfBusyFrames = count;
						for (k = j; k < j + count; k++)frameDescriptors[k].Attributes |= PMM_MEM_FRAME_ATTRIBUTE_BUSY;
						return (void*)(framesAddr + frameDescriptors[j].FrameIndex * PMM_MEM_FRAME_SIZE);
					}
				}
			}
		}
	}

	return 0;
}

void* pmm_realloc(void* oldMem, size_t size) {
	void* nMem = pmm_malloc(size);
	if (!nMem) return NULL;

	if (oldMem) {
		memcpy(nMem, oldMem, size);
		pmm_free(oldMem);
	}

	return nMem;
}

//	Returns number of frames
size_t pmm_free(void* oldMem) {
	MemoryFrameDescriptor_t* frameDescriptors;
	size_t framesCount, j, k, frameAddr;

	for (size_t i = 0; i < number_of_regions_; i++) {
		if (regions_[i].Type == ARD_TYPE_USABLE) {
			frameDescriptors = (MemoryFrameDescriptor_t*)(size_t)regions_[i].Address;
			framesCount = pmm_get_number_of_frame_descriptors(regions_[i].Size);
			frameAddr = PMM_ALIGN_UP((size_t)regions_[i].Address + sizeof(MemoryFrameDescriptor_t) * framesCount, PMM_MEM_FRAME_SIZE);

			for (j = 0; j < framesCount; j++) {
				if (frameAddr == (size_t)oldMem) {
					framesCount = frameDescriptors[j].NumberOfBusyFrames;
					frameDescriptors[j].NumberOfBusyFrames = 0;

					for (k = j; k < j + framesCount; k++) frameDescriptors[k].Attributes &= ~(size_t)PMM_MEM_FRAME_ATTRIBUTE_BUSY;

					return framesCount;
				}

				frameAddr += PMM_MEM_FRAME_SIZE;
			}
		}
	}

	return 0;
}