#include <_tty.h>
#include <_drive.h>
#include <_fpu.h>


extern void bootldr_main(unsigned char driveIndex) {
	tty_init(0xB8000, TTY_COLOR_LIGHT_GRAY, 80, 25, 160);
	if (!fpu_init()) tty_puts("Warning: failed to init FPU!\r\n");

	if (!drive_select(driveIndex)) {
		tty_puts("Error: failed to select drive!\r\n");
		__asm__ __volatile__("cli; hlt");
	}

	__asm__ __volatile__("cli; hlt");
}