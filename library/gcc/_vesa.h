#pragma once
#ifndef VESA_H
#define VESA_H

#include <stdint.h>
#include <string.h>
#include "_rm-srv.h"

typedef struct __attribute__((__packed__)) {
	uint32_t		Signature;
	uint16_t		Version;
	uint16_t		OEMStringsOffset;
	uint16_t		OEMStringsSegment;
	uint32_t		Capabilities;
	uint16_t		VideoModeOffset;
	uint16_t		VideoModeSegment;
	uint16_t		TotalMemorySize;		// in 64KB blocks
	uint8_t			Reserved[492];
} VESAInfo_t;

typedef struct __attribute__((__packed__)) {
	uint16_t		Attributes;				// VESA_MODE_ATTRIBUTE_*
	uint8_t			FirstWindow;
	uint8_t			SecondWindow;
	uint16_t		Granularity;
	uint16_t		WindowSize;
	uint16_t		FirstSegment;
	uint16_t		SecondSegment;
	uint32_t		WindowFunctionPointer;
	uint16_t		Pitch;					// bytes per scanline
	uint16_t		Width;
	uint16_t		Height;
	uint8_t			CharWidth;
	uint8_t			CharHeight;
	uint8_t			NumberOfPlanes;
	uint8_t			BitsPerPixel;
	uint8_t			NumberOfBanks;
	uint8_t			MemoryModel;			// VESA_MODE_MODEL_*
	uint8_t			BankSize;
	uint8_t			NumberOfImagePages;
	uint8_t			Reserved1;

	uint8_t			RedMaskSize;
	uint8_t			RedFieldPosition;
	uint8_t			GreenMaskSize;
	uint8_t			GreenFieldPosition;
	uint8_t			BlueMaskSize;
	uint8_t			BlueFieldPosition;
	uint8_t			ReservedMaskSize;
	uint8_t			ReservedFieldPosition;
	uint8_t			DirectColorModeInfo;

	// since VBE 2.0
	uint32_t		LFBPointer;
	uint32_t		OffscreenMemoryOffset;
	uint16_t		OffscreenMemorySize;	// in KB blocks
	uint8_t			Reserved2[206];
} VESAModeInfo_t;

uint8_t vesa_init(void);
VESAInfo_t* vesa_get_info(void);
VESAModeInfo_t* vesa_get_mode_info(uint16_t mode);
uint8_t vesa_set_mode(uint16_t mode);

#endif