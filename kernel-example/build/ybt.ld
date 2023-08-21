ENTRY(kernel_main)
OUTPUT_ARCH(i386)
OUTPUT_FORMAT(binary)
SECTIONS {
	__PTR_BASE_ADDR__ = 0x104000;
	. = __PTR_BASE_ADDR__;

	.yukiboot_header : {
		LONG(__PTR_BASE_ADDR__);		/* Base address */
		LONG(__PTR_END_ADDR__);			/* End address */
		LONG(__PTR_BASE_ADDR__);		/* Stack top address */
		LONG(kernel_main);				/* Entry point address */
	}

	.text BLOCK(4K) : ALIGN(4K) {
		*(.text)
	}

	.data BLOCK(4K) : ALIGN(4K) {
		*(.data)
	}

	.rodata BLOCK(4K) : ALIGN(4K) {
		*(.rodata)
	}

	.bss BLOCK(4K) : ALIGN(4K) {
		*(.bss)
	}

	__PTR_END_ADDR__ = .;
}