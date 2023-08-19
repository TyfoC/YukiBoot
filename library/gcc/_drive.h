#pragma once
#ifndef __DRIVE_H
#define __DRIVE_H

#include <stdint.h>
#include "_rm-srv.h"

uint8_t drive_select(uint8_t driveIndex);
uint16_t drive_get_spt(void);					// sectors per track
uint16_t drive_get_hpc(void);					// heads per cylinder
uint16_t drive_get_last_cylinder_index(void);
uint8_t drive_get_number_of_hdds(void);

#endif