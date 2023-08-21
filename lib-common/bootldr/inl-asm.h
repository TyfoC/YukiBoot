#pragma once
#ifndef INL_ASM_H
#define INL_ASM_H

#include <stdint.h>

inline uint8_t inb(uint16_t port) {
	uint8_t result;
	__asm__ __volatile__("inb %1, %0":"=a"(result):"Nd"(port):"memory");
    return result;
}

#endif