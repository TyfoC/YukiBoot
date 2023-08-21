#include "stdlib.h"

extern inline void* malloc(long unsigned int size);
extern inline void* calloc(long unsigned int count, long unsigned int size);
extern inline void* realloc(void* oldMem, long unsigned int newSize);
extern inline void free(void* oldMem);

static long unsigned int rand_next_ = 0;
static const char conv_digits_[36] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8',
	'9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q',
	'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
};

double atof(const char* src) {
	return strtod(src, NULL);
}

int atoi(const char* src) {
	int value = 0, isNeg = src[0] == '-';
	char* ptr = (char*)&src[isNeg || src[0] == '+'];

	while (*ptr && isdigit(*ptr)) {
		value = value * 10 + (*ptr - '0');
		++ptr;
	}

	if (isNeg) value = 0 - value;
	return value;
}

long atol(const char* src) {
	return strtol(src, 0);
}

double strtod(const char* src, char** endptr) {
	double value = 0.0, rightPartScale = 0.1;

	char* ptr = (char*)&src[0];
	while (*ptr == ' ') ++ptr;

	int isNeg = *ptr == '-';
	if (isNeg || *ptr == '+') ++ptr;

	while (*ptr && isdigit(*ptr)) {
		value = value * 10.0 + (double)(*ptr - '0');
		++ptr;
	}

	if (*ptr != '\0' && *ptr != '.') {
		if (endptr) *endptr = ptr;
		return value;
	}

	if (*ptr == '.') {
		++ptr;
		while (*ptr && isdigit(*ptr)) {
			value += rightPartScale * (double)(*ptr - '0');
			rightPartScale /= 10.0;
			++ptr;
		}

		if (*ptr != '\0'){
			if (endptr) *endptr = ptr;
			return value;
		}
	}

	if (isNeg) value = 0.0 - value;
	return value;
}

float strtof(const char* src, char** endptr) {
	float value = 0.0f, rightPartScale = 0.1f;

	char* ptr = (char*)&src[0];
	while (*ptr == ' ') ++ptr;

	int isNeg = *ptr == '-';
	if (isNeg || *ptr == '+') ++ptr;

	while (*ptr && isdigit(*ptr)) {
		value = value * 10.0f + (float)(*ptr - '0');
		++ptr;
	}

	if (*ptr != '\0' && *ptr != '.') {
		if (endptr) *endptr = ptr;
		return value;
	}

	if (*ptr == '.') {
		++ptr;
		while (*ptr && isdigit(*ptr)) {
			value += rightPartScale * (float)(*ptr - '0');
			rightPartScale /= 10.0f;
			++ptr;
		}

		if (*ptr != '\0'){
			if (endptr) *endptr = ptr;
			return value;
		}
	}

	if (isNeg) value = 0.0f - value;
	return value;
}

long strtol(const char* src, char** endptr) {
	long value = 0;

	char* ptr = (char*)&src[0];
	while (*ptr == ' ') ++ptr;

	int isNeg = *ptr == '-';
	if (isNeg || *ptr == '+') ++ptr;

	while (*ptr && isdigit(*ptr)) {
		value = value * 10 + *ptr - '0';
		++ptr;
	}

	if (*ptr != '\0') {
		if (endptr) *endptr = ptr;
		return value;
	}

	if (isNeg) value = 0 - value;
	return value;
}

unsigned long strtoul(const char* src, char** endptr) {
	unsigned long value = 0;

	char* ptr = (char*)&src[0];
	while (*ptr == ' ') ++ptr;
	if (*ptr == '+') ++ptr;

	while (*ptr && isdigit(*ptr)) {
		value = value * 10 + *ptr - '0';
		++ptr;
	}

	if (*ptr != '\0') {
		if (endptr) *endptr = ptr;
		return value;
	}

	return value;
}

int rand(void) {
	return(((rand_next_ = rand_next_ * 214013L + 2531011L) >> 16) & RAND_MAX);
}

void srand(int seed) {
	rand_next_ = seed;
}

int abs(int value) {
	return value < 0 ? 0 - value : value;
}

long labs(long value) {
	return value < 0 ? 0 - value : value;
}

long long llabs(long long value) {
	return value < 0 ? 0 - value : value;
}

div_t div(int x, int y) {
	div_t result;
	result.quot = x / y;
	result.rem = x % y;
	return result;
}

ldiv_t ldiv(long x, long y) {
	ldiv_t result;
	result.quot = x / y;
	result.rem = x % y;
	return result;
}

lldiv_t lldiv(long long x, long long y) {
	lldiv_t result;
	result.quot = x / y;
	result.rem = x % y;
	return result;
}

char* itoa(int value, char* buffer, int radix) {
	if (radix < 2 || radix > 36) return NULL;

	if (!value) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return buffer;
	}
	
	int isNeg = value < 0;
	char* buffPtr = (char*)&buffer[isNeg];
	if (isNeg) value = 0 - value;
	
	int valCopy = value;
	long unsigned int valLen = 1;
	while (valCopy /= radix) ++valLen;
	
	char* ptr = (char*)&buffPtr[valLen];
	*ptr-- = '\0';
	
	while (value) {
	    *ptr-- = conv_digits_[value % radix];
	    value /= radix;
	}

	++ptr;
	if (isNeg) *ptr = '-';
	return ptr;
}

char* ltoa(long value, char* buffer, int radix) {
	if (radix < 2 || radix > 36) return NULL;

	if (!value) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return buffer;
	}
	
	int isNeg = value < 0;
	char* buffPtr = (char*)&buffer[0];
	if (isNeg) {
	    ++buffPtr;
	    value = 0 - value;
	}
	
	long valCopy = value;
	long unsigned int valLen = 1;
	while (valCopy /= radix) ++valLen;
	
	char* ptr = (char*)&buffPtr[valLen];
	*ptr-- = '\0';
	
	while (value) {
	    *ptr-- = conv_digits_[value % radix];
	    value /= radix;
	}

	if (isNeg) *ptr = '-';
	return ++ptr;
}

char* lltoa(long long value, char* buffer, int radix) {
	if (radix < 2 || radix > 36) return NULL;

	if (!value) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return buffer;
	}
	
	int isNeg = value < 0;
	char* buffPtr = (char*)&buffer[0];
	if (isNeg) {
	    ++buffPtr;
	    value = 0 - value;
	}
	
	long long valCopy = value;
	long unsigned int valLen = 1;
	while (valCopy /= radix) ++valLen;
	
	char* ptr = (char*)&buffPtr[valLen];
	*ptr-- = '\0';
	
	while (value) {
	    *ptr-- = conv_digits_[value % radix];
	    value /= radix;
	}

	if (isNeg) *ptr = '-';
	return ++ptr;
}

char* utoa(unsigned int value, char* buffer, int radix) {
	if (radix < 2 || radix > 36) return NULL;

	if (!value) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return buffer;
	}

	long unsigned int valLen = 1;
	unsigned int valCopy = value;
	while (valCopy /= radix) ++valLen;
	
	char* ptr = (char*)&buffer[valLen];
	*ptr-- = '\0';
	
	while (value) {
	    *ptr-- = conv_digits_[value % radix];
	    value /= radix;
	}

	return ++ptr;
}

char* ultoa(unsigned long value, char* buffer, int radix) {
	if (radix < 2 || radix > 36) return NULL;

	if (!value) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return buffer;
	}

	long unsigned int valLen = 1;
	unsigned long valCopy = value;
	while (valCopy /= radix) ++valLen;
	
	char* ptr = (char*)&buffer[valLen];
	*ptr-- = '\0';
	
	while (value) {
	    *ptr-- = conv_digits_[value % radix];
	    value /= radix;
	}

	return ++ptr;
}

char* ulltoa(unsigned long long value, char* buffer, int radix) {
	if (radix < 2 || radix > 36) return NULL;

	if (!value) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return buffer;
	}

	long unsigned int valLen = 1;
	unsigned long long valCopy = value;
	while (valCopy /= radix) ++valLen;
	
	char* ptr = (char*)&buffer[valLen];
	*ptr-- = '\0';
	
	while (value) {
	    *ptr-- = conv_digits_[value % radix];
	    value /= radix;
	}

	return ++ptr;
}

char* ftoa(float value, char* buffer, int radix) {
	if (radix < 2 || radix > 36) return NULL;
	
	if (value < 0.0f) {
		*buffer++ = '-';
		value = 0.0f - value;
	}

	int leftPart = (int)value;
	float rightPart = value - (float)leftPart;

	itoa(leftPart, buffer, radix);
	char* ptr = (char*)&buffer[strlen(buffer)];

	size_t tmpPrec = FFUNCS_PREC;
	if (rightPart) {
		*ptr++ = '.';
		while (rightPart && tmpPrec) {
			leftPart = (int)(rightPart *= (float)radix);
			*ptr++ = conv_digits_[leftPart % radix];
			rightPart -= (float)leftPart;
			--tmpPrec;
		}
		*ptr = '\0';
	}

	return buffer;
}

char* dtoa(double value, char* buffer, int radix) {
	if (radix < 2 || radix > 36) return NULL;
	
	if (value < 0.0) {
		*buffer++ = '-';
		value = 0.0 - value;
	}

	long long leftPart = (long long)value;
	double rightPart = value - (double)leftPart;

	itoa(leftPart, buffer, radix);
	char* ptr = (char*)&buffer[strlen(buffer)];

	size_t tmpPrec = FFUNCS_PREC;
	if (rightPart) {
		*ptr++ = '.';
		while (rightPart && tmpPrec) {
			leftPart = (long long)(rightPart *= (double)radix);
			*ptr++ = conv_digits_[leftPart % radix];
			rightPart -= (double)leftPart;
			--tmpPrec;
		}
		*ptr = '\0';
	}

	return buffer;
}

char* ldtoa(long double value, char* buffer, int radix) {
	if (radix < 2 || radix > 36) return NULL;
	
	if (value < 0.0L) {
		*buffer++ = '-';
		value = 0.0L - value;
	}

	long long leftPart = (long long)value;
	long double rightPart = value - (long double)leftPart;

	itoa(leftPart, buffer, radix);
	char* ptr = (char*)&buffer[strlen(buffer)];

	size_t tmpPrec = FFUNCS_PREC;
	if (rightPart) {
		*ptr++ = '.';
		while (rightPart && tmpPrec) {
			leftPart = (long long)(rightPart *= (long double)radix);
			*ptr++ = conv_digits_[leftPart % radix];
			rightPart -= (long double)leftPart;
			--tmpPrec;
		}
		*ptr = '\0';
	}

	return buffer;
}

long uratol(const char* str) {
	long result = 0;

	char* buffPtr = (char*)&str[0];
	uint8_t isNeg = buffPtr[0] == '-';
	if (isNeg || buffPtr[0] == '+') ++buffPtr;

	if (buffPtr[0] != '0' || buffPtr[1] == '\0') return strtol(str, NULL);
	++buffPtr;

	size_t radix = 10;
	if (toupper(*buffPtr) == 'X') radix = 16;
	else if (toupper(*buffPtr) == 'B') radix = 2;
	else radix = 8;
	++buffPtr;

	char tmp;
	while (*buffPtr) {
		tmp = toupper(*buffPtr);
		++buffPtr;

		result *= radix;
		if (isdigit(tmp)) result += tmp - '0';
		else result += tmp - 'A';
	}

	if (isNeg) result = 0 - result;
	return result;
}