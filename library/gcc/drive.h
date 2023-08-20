#pragma once
#ifndef DRIVE_H
#define DRIVE_H

#include <stdint.h>
#include "string.h"
#include "rm-srv.h"

#define JOIN_CYL_SECT($cyl, $sect)		(uint16_t)((($cyl & 0xFF) << 8) | (($cyl & 0x300) >> 2) | ($sect & 0x3F))
#define JOIN_HEAD_DRIVE($head, $drive)	(uint16_t)(($head << 8) | ($drive & 0xFF))

#define DRIVE_SECTOR_SIZE				512

#define DRIVE_IF_SUPPORT_DAP					0x01
#define DRIVE_IF_SUPPORT_LOCKING_AND_EJECTING	0x02
#define DRIVE_IF_SUPPORT_EDD					0x04

uint8_t drive_select(uint8_t driveIndex);
uint16_t drive_get_spt(void);					// sectors per track
uint16_t drive_get_hpc(void);					// heads per cylinder
uint16_t drive_get_last_cylinder_index(void);
uint8_t drive_get_number_of_hdds(void);
uint16_t drive_get_interface_support_bitmask(void);
uint32_t drive_get_max_lba(void);

void drive_lba_to_chs(uint32_t lba, uint16_t* cylinder, uint8_t* head, uint8_t* sector);
size_t drive_read_sectors(uint32_t lba, size_t count, void* buffer);
size_t drive_read_sectors_ext(uint64_t lba, size_t count, void* buffer);

#endif