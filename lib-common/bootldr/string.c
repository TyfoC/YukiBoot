#include "string.h"

void* memcpy(void* dst, const void* src, size_t size) {
	unsigned char* dst8 = (unsigned char*)dst;
	unsigned char* src8 = (unsigned char*)src;

	for (size_t i = 0; i < size; i++) dst8[i] = src8[i];

	return (void*)dst8;
}

void* memset(void* dst, unsigned char value, size_t size) {
	unsigned char* dst8 = (unsigned char*)dst;

	while (size) {
		*dst8 = value;

		++dst8;
		--size;
	}

	return (void*)dst8;
}

void* memchr(const void* src, char value, size_t size) {
	char* ptr = (char*)src;

	while (size) {
		if (*ptr == value) return ptr;

		++ptr;
		--size;
	}

	return NULL;
}

int memcmp(void* first, const void* second, size_t size) {
	unsigned char* first8 = (unsigned char*)first;
	unsigned char* second8 = (unsigned char*)second;

	while (size) {
		if (*first8 < *second8) return -1;
		else if (*first8 > *second8) return 1;

		++first8;
		++second8;
		--size;
	}

	return 0;
}

char* strcat(char* dst, const char* src) {
	size_t dstLen = strlen(dst), srcLen = strlen(src);
	return memcpy(&dst[dstLen], &src[0], srcLen + 1);
}

char* strncat(char* dst, const char* src, size_t n) {
	size_t dstLen = strlen(dst), srcLen = strlen(src);
	if (srcLen < n) n = srcLen;

	dst[dstLen + n] = '\0';
	return memcpy(&dst[dstLen], &src[0], n);
}

char* strchr(const char* src, char value) {
	char* ptr = (char*)src;

	while (*ptr) {
		if (*ptr == value) return ptr;

		++ptr;
	}

	return NULL;
}

char* strrchr(const char* src, char value) {
	size_t srcLen = strlen(src);
	if (!srcLen) return NULL;

	char* ptr = (char*)&src[srcLen - 1];
	size_t rightBndAddr = (size_t)ptr;

	while ((size_t)ptr >= (size_t)src && (size_t)ptr <= rightBndAddr) {
		if (*ptr == value) return ptr;

		--ptr;
	}

	return NULL;
}

int strcmp(const char* first, const char* second) {
	while (*first && (*first == *second)) {
		++first;
		++second;
	}

	return *(unsigned char*)first - *(unsigned char*)second;
}

int strncmp(const char* first, const char* second, size_t n) {
	while (*first && (*first == *second) && n) {
		++first;
		++second;
		--n;
	}

	return *(unsigned char*)first - *(unsigned char*)second;
}

char* strcpy(char* dst, const char* src) {
	return (char*)memcpy(dst, src, strlen(src) + 1);
}

char* strncpy(char* dst, const char* src, size_t n) {
	size_t srcLen = strlen(src);
	if (srcLen < n) n = srcLen;

	memcpy(dst, src, n);
	dst[n] = '\0';

	return dst;
}

size_t strlen(const char* src) {
	size_t result = 0;
	while (src[result]) ++result;
	return result;
}

size_t strspn(const char* src, const char* accept) {
	char* ptr = (char*)&src[0];
	while (*ptr && strchr(accept, *ptr)) ++ptr;
	return (size_t)ptr - (size_t)src;
}

size_t strcspn(const char* src, const char* reject) {
	char* ptr = (char*)&src[0];
	while (*ptr && !strchr(reject, *ptr)) ++ptr;
	return (size_t)ptr - (size_t)src;
}

char* strpbrk(const char* src, const char* accept) {
	char* acceptPtr = (char*)&accept[0];
	char* result = NULL;

	while (*acceptPtr) {
		result = strchr(src, *acceptPtr++);
		if (result) return result;
	}

	return result;
}

char* strstr(const char* haystack, const char* needle) {
	size_t needleLen = strlen(needle);
	char* ptr = (char*)&haystack[0];
	
	while (*ptr) {
		if (!memcmp(ptr, needle, needleLen)) return ptr;
		++ptr;
	}

	return NULL;
}

char* strrev(char* src) {
	size_t strLen = strlen(src);
	if (!strLen) return src;

	char tmp;
	char* begPtr = (char*)&src[0];
	char* endPtr = (char*)&src[strLen - 1];
	
	while ((size_t)begPtr < (size_t)endPtr) {
		tmp = *begPtr;
		*begPtr++ = *endPtr;
		*endPtr++ = tmp;
	}

	return &src[0];
}