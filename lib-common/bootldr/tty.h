#pragma once
#ifndef TTY_H
#define TTY_H

#include <stdint.h>
#include <stdarg.h>
#include "stdlib.h"
#include "string.h"

#define TTY_COLOR_BLACK				0x00
#define TTY_COLOR_BLUE				0x01
#define TTY_COLOR_GREEN				0x02
#define TTY_COLOR_CYAN				0x03
#define TTY_COLOR_RED				0x04
#define TTY_COLOR_MAGENTA			0x05
#define TTY_COLOR_BROWN				0x06
#define TTY_COLOR_LIGHT_GRAY		0x07
#define TTY_COLOR_DARK_GRAY			0x08
#define TTY_COLOR_LIGHT_BLUE		0x09
#define TTY_COLOR_LIGHT_GREEN		0x0A
#define TTY_COLOR_LIGHT_CYAN		0x0B
#define TTY_COLOR_LIGHT_RED			0x0C
#define TTY_COLOR_YELLOW			0x0E
#define TTY_COLOR_WHITE				0x0F

#define TTY_TAB_LENGTH				0x04

void tty_init(uint32_t videoBufferAddress, uint8_t textColor, uint16_t numOfColumns, uint16_t numOfLines, uint16_t bytesPerScanLine);

void tty_set_video_buffer_address(uint32_t videoBufferAddress);
void tty_set_text_color(uint8_t textColor);
void tty_set_number_of_lines(uint16_t numOfLines);
void tty_set_number_of_columns(uint16_t numOfColumns);
void tty_set_bytes_per_scan_line(uint16_t bytesPerScanLine);
uint32_t tty_get_video_buffer_address(void);
uint8_t tty_get_text_color(void);
uint16_t tty_get_number_of_lines(void);
uint16_t tty_get_number_of_columns(void);
uint16_t tty_get_bytes_per_scan_line(void);

void tty_set_line(uint16_t lineIndex);
void tty_set_column(uint16_t columnIndex);
uint16_t tty_get_line(void);
uint16_t tty_get_column(void);

void tty_fix_cursor_position(void);

void tty_clear_screen(void);

void tty_putc(char value);
void tty_puts(const char* str);

void tty_vprintf(const char* fmt, va_list list);
inline void tty_printf(const char* fmt, ...) {
	va_list arguments;
	va_start(arguments, fmt);
	tty_vprintf(fmt, arguments);
}

#endif