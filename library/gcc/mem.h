#pragma once
#ifndef MEM_H
#define MEM_H

#include "stdint.h"
#include "string.h"
#include "rm-srv.h"

typedef struct __attribute__((__packed__)) {
	uint64_t		Address;
	uint64_t		Size;
	uint32_t		Type;
	uint32_t		Attributes;
} AddressRangeDescriptor_t;

size_t mem_get_map(void* buffer);

#endif