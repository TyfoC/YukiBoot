#pragma once
#ifndef _STDIO_H
#define _STDIO_H

#include "tty.h"

inline void putchar(char character) {
	tty_putc(character);
}

inline void puts(const char* str) {
	for (size_t i = 0; str[i]; i++) putchar(str[i]);
}

inline void vprintf(const char* fmt, va_list list) {
	tty_vprintf(fmt, list);
}

inline void printf(const char* fmt, ...) {
	va_list list;
	va_start(list, fmt);
	vprintf(fmt, list);
}

#endif