#include <_sib.h>
#include <_tty.h>
#include <_drive.h>
#include <_fpu.h>
#include <_vesa.h>

SystemInfoBlock_t SystemInfoBlock __attribute__((__aligned__(8)));
char buffer[1024];

extern void bootldr_main(unsigned char driveIndex) {
	SystemInfoBlock.DriveIndex = driveIndex;
	tty_init(0xB8000, TTY_COLOR_LIGHT_GRAY, 80, 25, 160);

	if (!drive_select(SystemInfoBlock.DriveIndex)) {
		tty_printf("Error: failed to select drive (0x%x)!\r\n", SystemInfoBlock.DriveIndex);
		__asm__ __volatile__("cli; hlt");
	}

	SystemInfoBlock.NumberOfHDDs = drive_get_number_of_hdds();

	if (!vesa_init()) tty_puts("Warning: failed to init VESA!\r\n");
	else {
		memcpy(&SystemInfoBlock.VESAInfo, vesa_get_info(), sizeof(VESAInfo_t));
		tty_puts("VESA signature: `");
		tty_putc(SystemInfoBlock.VESAInfo.Signature & 0xFF);
		tty_putc((SystemInfoBlock.VESAInfo.Signature >> 8) & 0xFF);
		tty_putc((SystemInfoBlock.VESAInfo.Signature >> 16) & 0xFF);
		tty_putc((SystemInfoBlock.VESAInfo.Signature >> 24) & 0xFF);
		tty_printf("`\r\nVESA version: %u.%u\r\n", SystemInfoBlock.VESAInfo.Version >> 8, SystemInfoBlock.VESAInfo.Version & 0xFF);
	}

	__asm__ __volatile__("cli; hlt"::"a"(SystemInfoBlock.VESAInfo.Signature));
}