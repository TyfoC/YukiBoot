#include "tty.h"

extern inline void tty_printf(const char* fmt, ...);

static uint32_t video_buffer_address_ = 0xB8000;
static uint8_t text_color_ = TTY_COLOR_LIGHT_GRAY;
static uint16_t number_of_columns_ = 80;
static uint16_t number_of_lines_ = 25;
static uint16_t bytes_per_scan_line_ = 160;
static uint32_t cursor_offset_ = 0;
static char conv_buffer_[66];

void tty_init(uint32_t videoBufferAddress, uint8_t textColor, uint16_t numOfColumns, uint16_t numOfLines, uint16_t bytesPerScanLine) {
	video_buffer_address_ = videoBufferAddress;
	text_color_ = textColor;
	number_of_columns_ = numOfColumns;
	number_of_lines_ = numOfLines;
	bytes_per_scan_line_ = bytesPerScanLine;
	cursor_offset_ = 0;
}

void tty_set_video_buffer_address(uint32_t videoBufferAddress) {
	video_buffer_address_ = videoBufferAddress;
}

void tty_set_text_color(uint8_t textColor) {
	text_color_ = textColor;
}

void tty_set_number_of_lines(uint16_t numOfLines) {
	number_of_lines_ = numOfLines;
}

void tty_set_number_of_columns(uint16_t numOfColumns) {
	number_of_columns_ = numOfColumns;
}

void tty_set_bytes_per_scan_line(uint16_t bytesPerScanLine) {
	bytes_per_scan_line_ = bytesPerScanLine;
}

uint32_t tty_get_video_buffer_address(void) {
	return video_buffer_address_;
}

uint8_t tty_get_text_color(void) {
	return text_color_;
}

uint16_t tty_get_number_of_lines(void) {
	return number_of_lines_;
}

uint16_t tty_get_number_of_columns(void) {
	return number_of_columns_;
}

uint16_t tty_get_bytes_per_scan_line(void) {
	return bytes_per_scan_line_;
}

void tty_set_line(uint16_t lineIndex) {
	cursor_offset_ = lineIndex * bytes_per_scan_line_ + ((cursor_offset_ % bytes_per_scan_line_) >> 1);
}

void tty_set_column(uint16_t columnIndex) {
	if (columnIndex >= number_of_columns_) return;

	cursor_offset_ = cursor_offset_ - (cursor_offset_ % bytes_per_scan_line_) + (columnIndex << 1);
}

uint16_t tty_get_line(void) {
	return cursor_offset_ / bytes_per_scan_line_;
}

uint16_t tty_get_column(void) {
	return (cursor_offset_ % bytes_per_scan_line_) >> 1;
}

void tty_fix_cursor_position(void) {
	if (!number_of_lines_) {
		cursor_offset_ = 0;
		return;
	}

	if (tty_get_line() >= (uint32_t)number_of_lines_) {
		const size_t lineWidth = number_of_columns_ << 1;
		void* dstBuff = (void*)video_buffer_address_;
		void* srcBuff = (void*)(video_buffer_address_ + bytes_per_scan_line_);

		for (size_t i = 0; i < (size_t)(number_of_lines_ - 1); i++) {;
			memcpy(dstBuff, srcBuff, lineWidth);
			dstBuff += bytes_per_scan_line_;
			srcBuff += bytes_per_scan_line_;
		}
		
		memset(dstBuff, 0, lineWidth);
		tty_set_line(number_of_lines_ - 1);
		tty_set_column(0);
	}
}

void tty_putc(char value) {
	if (!number_of_lines_) return;

	tty_fix_cursor_position();

	if (value == '\r') tty_set_column(0);
	else if (value == '\n') tty_set_line(tty_get_line() + 1);
	else if (value == '\t') for (size_t i = 0; i < TTY_TAB_LENGTH; i++) tty_putc(' ');
	else if (value == '\n') for (size_t i = 0; i < TTY_TAB_LENGTH; i++) tty_putc('\n');
	else if (value == '\b') {
		if (cursor_offset_ >= 2) cursor_offset_ -= 2;
	}
	else {
		((char*)video_buffer_address_)[cursor_offset_++] = value;
		((char*)video_buffer_address_)[cursor_offset_++] = text_color_;
	}
}

void tty_puts(const char* str) {
	char* ptr = (char*)&str[0];
	while (*ptr) tty_putc(*ptr++);
}

/*
	%[flags][-width][...]

	flags:
		`+` - print sign for decimal number (dec)
		`#` - alternative form for printing the value (hex, oct)
*/
void tty_vprintf(const char* format, va_list list) {
	char* ptr = (char*)&format[0];
	uint8_t printSign, altForm;
	
	while (*ptr) {
		if (*ptr == '%') {
			printSign = altForm = 0;

			while (1) {
				if (*(++ptr) == '+') printSign = 1;
				else if (*ptr == '#') altForm = 1;
				else break;
			}

			if (*ptr == '%') tty_putc('%');
			else if (*ptr == 'c') tty_putc((char)va_arg(list, int));
			else if (*ptr == 's') tty_puts(va_arg(list, char*));
			else if (*ptr == 'd') {
				int32_t tmp = va_arg(list, int32_t);
				if (printSign && tmp >= 0) tty_putc('+');
				tty_puts(itoa(tmp, conv_buffer_, 10));
			}
			else if (*ptr == 'u') tty_puts(utoa(va_arg(list, uint32_t), conv_buffer_, 10));
			else if (*ptr == 'b') tty_puts(utoa(va_arg(list, uint32_t), conv_buffer_, 2));
			else if (*ptr == 'o') {
				if (altForm) tty_putc('0');
				tty_puts(utoa(va_arg(list, uint32_t), conv_buffer_, 8));
			}
			else if (*ptr == 'x') {
				if (altForm) tty_puts("0x");
				tty_puts(utoa(va_arg(list, uint32_t), conv_buffer_, 16));
			}
			else if (*ptr == 'f') {
				tty_puts(dtoa(va_arg(list, double), conv_buffer_, 10));
			}
			else if (*ptr == 'l') {
				if (*(++ptr) == 'd') {
					long tmp = va_arg(list, long);
					if (printSign && tmp >= 0) tty_putc('+');
					tty_puts(ltoa(tmp, conv_buffer_, 10));
				}
				else if (*ptr == 'u') tty_puts(ultoa(va_arg(list, unsigned long), conv_buffer_, 10));
				else if (*ptr == 'l') {
					if (*(++ptr) == 'd')  {
						long long tmp = va_arg(list, long long);
						if (printSign && tmp >= 0) tty_putc('+');
						tty_puts(lltoa(tmp, conv_buffer_, 10));
					}
					else if (*ptr == 'u') tty_puts(ulltoa(va_arg(list, unsigned long long), conv_buffer_, 10));
				}
			}
		}
		else tty_putc(*ptr);
		++ptr;
	}

	va_end(list);
}