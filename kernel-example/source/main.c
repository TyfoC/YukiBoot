#include <sib.h>

extern void kernel_main(const SystemInfoBlock_t* sib) {
	uint8_t* videoBuffer = (uint8_t*)sib->VideoBufferAddress;
	const size_t width = sib->VideoModeWidth;
	const size_t widthSize = width * (sib->VideoModeBitsPerPixel / 8);

	size_t tmpBase = 0;
	uint32_t lineOffset = 0, columnOffset;
	uint8_t colorRed = 0x72, colorGreen = 0x9B, colorBlue = 0xF8;
	while (1) {
		for (size_t y = 0; y < sib->VideoModeHeight; y++) {
			for (columnOffset = 0; columnOffset < widthSize;) {
				videoBuffer[lineOffset + columnOffset++] = colorBlue;
				videoBuffer[lineOffset + columnOffset++] = colorGreen;
				videoBuffer[lineOffset + columnOffset++] = colorRed;

				colorBlue += columnOffset;
				colorGreen -= lineOffset + columnOffset * 2;
				colorRed -= (columnOffset << 8) + lineOffset * 3;
			}

			lineOffset += 800 * 3;
			columnOffset = 0;
		}

		lineOffset = 0;
		++tmpBase;

		colorRed += tmpBase, colorGreen -= tmpBase, colorBlue += tmpBase;
	}
}