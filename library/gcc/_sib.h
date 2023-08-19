#pragma once
#ifndef __SIB_H
#define __SIB_H						// system info block

#include <stdint.h>
#include "_vesa.h"

typedef struct {
	uint32_t		DriveIndex;		// BIOS drive index
	uint32_t		NumberOfHDDs;	// number of hard disk drives
	VESAInfo_t		VESAInfo;		// VESA info
} SystemInfoBlock_t;

#endif