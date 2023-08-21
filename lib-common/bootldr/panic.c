#include "panic.h"

void panic(const char* fmtMsg, ...) {
	tty_set_text_color(TTY_COLOR_LIGHT_RED);
	puts("yukiboot panic:\r\n");

	va_list list;
	va_start(list, fmtMsg);
	vprintf(fmtMsg, list);

	__asm__ __volatile__("cli; hlt");
}