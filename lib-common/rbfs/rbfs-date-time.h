#pragma once
#ifndef RBFS_DATE_TIME_H
#define RBFS_DATE_TIME_H

#include "rbfs-typedefs.h"

/*
	Describes the time.
*/
typedef struct __attribute__((__packed__)) {
	uint32_t	Day		:5;
	uint32_t	Month	:4;
	uint32_t	Year	:23;
} RBFSDate_t;

/*
	Describes the date.
*/
typedef struct __attribute__((__packed__)) {
	uint16_t	Seconds		:6;
	uint16_t	Minutes		:6;
	uint16_t	Hours		:5;
	uint16_t	Reserved	:15;
} RBFSTime_t;

#endif