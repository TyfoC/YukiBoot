ENTRY(bootldr_main)
OUTPUT_ARCH(i386)
OUTPUT_FORMAT(binary)
SECTIONS {
	__PTR_BASE_ADDR__ = 0x00001400;
	. = __PTR_BASE_ADDR__;

	.header : {
		/*LONG(((__PTR_END_ADDR__ - __PTR_BASE_ADDR__ + 511) & ~511) / 512); - length in sectors (deprecated) */
		LONG(bootldr_main);														/* entry point */
	}

	.text : {
		*(.text)
	}

	.data BLOCK(512) : ALIGN(512) {
		*(.data)
	}

	.rodata BLOCK(512) : ALIGN(512) {
		*(.rodata)
	}

	.bss BLOCK(512) : ALIGN(512) {
		*(.bss)
	}

	.padding BLOCK(512) : ALIGN(512) {
		__PTR_END_ADDR__ = .;
	}
}