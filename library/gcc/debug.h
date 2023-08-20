#pragma once
#ifndef DEBUG_H
#define DEBUG_H

#include "stdio.h"

#ifdef DEBUG
#define DBG_PUTCHAR($character)	putchar($character)
#define DBG_PUTS($str)			puts($str)
#define DBG_PRINTF($fmt, ...)	printf($fmt, __VA_ARGS__)
#else
#define DBG_PUTCHAR($character)	((void)$character)
#define DBG_PUTS($str)			((void)$str)
#define DBG_PRINTF($fmt, ...)	((void)$fmt)
#endif

#endif