#pragma once
#ifndef _STDLIB_H
#define _STDLIB_H					// stdlib.h additional functions

#include <stddef.h>
#include "ctype.h"
#include "string.h"
#include "_fpu.h"

#define RAND_MAX						32767

#define FFUNCS_PREC						8				// ftoa, dtoa, ldtoa

typedef struct __attribute__((__packed__)) {
	int			quot;
	int			rem;
} div_t;

typedef struct __attribute__((__packed__)) {
	long		quot;
	long		rem;
} ldiv_t;

typedef struct __attribute__((__packed__)) {
	long long	quot;
	long long	rem;
} lldiv_t;

//	Type conversion functions

double atof(const char* src);
int atoi(const char* src);
long atol(const char* src);
double strtod(const char* src, char** endptr);
float strtof(const char* src, char** endptr);
long strtol(const char* src, char** endptr);
unsigned long strtoul(const char* src, char** endptr);

//	Functions for generating pseudo-random sequences

int rand(void);
void srand(int seed);

// [NO_IMPL] Memory allocation and deallocation functions (only prototypes, must be implemented in the kernel)

void* malloc(long unsigned int size);
void* calloc(long unsigned int count, long unsigned int elemSize);
void* realloc(void* oldMemory, long unsigned int newSize);
void free(void* oldMemory);

//	Math functions

int abs(int value);
long labs(long value);
long long llabs(long long value);
div_t div(int x, int y);
ldiv_t ldiv(long x, long y);
lldiv_t lldiv(long long x, long long y);

//	Non-standard functions

char* itoa(int value, char* buffer, int radix);
char* ltoa(long value, char* buffer, int radix);
char* lltoa(long long value, char* buffer, int radix);
char* utoa(unsigned int value, char* buffer, int radix);
char* ultoa(unsigned long value, char* buffer, int radix);
char* ulltoa(unsigned long long value, char* buffer, int radix);
char* ftoa(float value, char* buffer, int radix);
char* dtoa(double value, char* buffer, int radix);
char* ldtoa(long double value, char* buffer, int radix);

#endif