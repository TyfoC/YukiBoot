#pragma once
#ifndef _STRING_H
#define _STRING_H				// string.h implementation

#include <stddef.h>

typedef __SIZE_TYPE__ size_t;
typedef __PTRDIFF_TYPE__ ptrdiff_t;

void* memcpy(void* dst, const void* src, size_t size);
void* memset(void* dst, unsigned char value, size_t size);
void* memchr(const void* src, char value, size_t size);
int memcmp(void* first, const void* second, size_t size);
char* strcat(char* dst, const char* src);
char* strncat(char* dst, const char* src, size_t n);
char* strchr(const char* src, char value);
char* strrchr(const char* src, char value);
int strcmp(const char* first, const char* second);
int strncmp(const char* first, const char* second, size_t n);
char* strcpy(char* dst, const char* src);
char* strncpy(char* dst, const char* src, size_t n);
size_t strlen(const char* src);
size_t strspn(const char* src, const char* accept);
size_t strcspn(const char* src, const char* reject);
char* strpbrk(const char* src, const char* accept);
char* strstr(const char* haystack, const char* needle);
char* strrev(char* src);

#endif							// string.h implementation