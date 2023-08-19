#pragma once
#ifndef __FPU_H
#define __FPU_H

#include <stdint.h>

#define FPU_FILD(...)				__asm__ __volatile__("fild %0"::"m"(__VA_ARGS__))
#define FPU_FLD(...)				__asm__ __volatile__("fld %0"::"m"(__VA_ARGS__))
#define FPU_FISTP(...)				__asm__ __volatile__("fistp %0":"=m"(__VA_ARGS__))
#define FPU_FSTP(...)				__asm__ __volatile__("fstp %0":"=m"(__VA_ARGS__))

#define FPU_F2I($floatSrc, $intDst)	__asm__ __volatile__("fld %1; fistp %0":"=m"($intDst):"m"($floatSrc))

extern uint8_t fpu_init(void);

#endif