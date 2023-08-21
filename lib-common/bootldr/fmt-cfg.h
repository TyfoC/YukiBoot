#pragma once
#ifndef FMT_CFG_H
#define FMT_CFG_H

#include "stdio.h"

typedef struct __attribute__((__packed__)) {
	char*	Name;
	char*	Value;
} ConfigFormatUnit_t;

void fmt_cfg_conv_str_to_units(const char* str, ConfigFormatUnit_t** units, size_t* count);
void fmt_cfg_free_units(ConfigFormatUnit_t* units, size_t count);
char* fmt_cfg_get_value(const ConfigFormatUnit_t* units, size_t count, const char* name);

#endif