#pragma once
#ifndef _CTYPE_H
#define _CTYPE_H

#include <stdint.h>
#include <stdbool.h>

#define __CTYPE_PUNCT_CHARACTERS_COUNT	UINT8_C(32)

extern const char punct_characters_[__CTYPE_PUNCT_CHARACTERS_COUNT];

//	Character classification

inline int isdigit(char value) {
	return value >= '0' && value <= '9';
}

inline int isalpha(char value) {
	return (
		(value >= 'A' && value <= 'Z') ||
		(value >= 'a' && value <= 'z')
	);
}

inline int isalnum(char value) {
	return isalpha(value) || isdigit(value);
}

inline int islower(char value) {
	return value >= 'a' && value <= 'z';
}

inline int isupper(char value) {
	return value >= 'A' && value <= 'Z';
}

inline int isxdigit(char value) {
	return (
		isdigit(value) ||
		(value >= 'a' && value <= 'f') ||
		(value >= 'A' && value <= 'F')
	);
}

inline int iscntrl(char value) {
	return value < 0x20 || value == 0x7F;
}

inline int ispunct(char value) {
	for (uint8_t i = 0; i < __CTYPE_PUNCT_CHARACTERS_COUNT; i++) if (value == punct_characters_[i]) return true;
	return false;
}

inline int isgraph(char value) {
	return isalnum(value) || ispunct(value);
}

inline int isspace(char value) {
	return (
		(value >= '\t' && value <= '\r') ||
		value == ' '
	);
}

inline int isblank(char value) {
	return value == ' ' || value =='\t';
}

inline int isprint(char value) {
	return isalnum(value) || ispunct(value);
}

//	Character manipulation

inline int tolower(char value) {
	return isupper(value) ? value | ' ' : value;
}

inline int toupper(char value) {
	return islower(value) ? value & '_' : value;
}

#endif