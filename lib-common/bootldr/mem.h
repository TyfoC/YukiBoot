#pragma once
#ifndef MEM_H
#define MEM_H

#include "stdint.h"
#include "string.h"
#include "rm-srv.h"

#define ARD_TYPE_USABLE						0x00000001
#define ARD_TYPE_RESERVED					0x00000002
#define ARD_TYPE_RECLAIMABLE				0x00000003
// ...

#define ARD_ATTRIBUTE_PRESENT				0x00000001

typedef struct __attribute__((__packed__)) {
	uint64_t		Address;
	uint64_t		Size;
	uint32_t		Type;					// ARD_TYPE_*
	uint32_t		Attributes;				// ARD_ATTRIBUTE_*
} AddressRangeDescriptor_t;

size_t mem_get_map(void* buffer);

#endif