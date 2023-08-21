#include "fmt-cfg.h"

void fmt_cfg_conv_str_to_units(const char* str, ConfigFormatUnit_t** units, size_t* count) {
	ConfigFormatUnit_t tmpUnit;

	char* beg;
	char* end;
	char* dst;
	size_t tmpLen, i, j;
	uint8_t dstName = 1;
	char* ptr = (char*)&str[0];
	while (*ptr) {
		while (isspace(*ptr)) ++ptr;
		if (!*ptr) {
			if (!dstName) free(tmpUnit.Name);
			break;
		}

		if (*ptr == '#') while (*ptr != '\r' && *ptr != '\n') ++ptr;
		if (!*ptr) {
			if (!dstName) free(tmpUnit.Name);
			break;
		}

		if (*ptr == '"') {
			beg = ++ptr;
			while (*ptr != '"' || *(ptr - 1) == '\\') ++ptr;
			end = ++ptr;

			tmpLen = (size_t)end - (size_t)beg - 1;

			if (dstName) {
				tmpUnit.Name = (char*)malloc(tmpLen + 1);
				dst = (char*)&tmpUnit.Name[0];
				if (!dst) continue;
			}
			else {
				tmpUnit.Value = (char*)malloc(tmpLen + 1);
				dst = (char*)&tmpUnit.Value[0];
				if (!dst) {
					free(tmpUnit.Name);
					continue;
				}
			}

			for (i = 0, j = 0; i < tmpLen; i++) {
				if (beg[i] == '\\') {
					++i;
					if (beg[i] == '\\') dst[j++] = '\\';
					else if (beg[i] == '\"') dst[j++] = '\"';
					else if (beg[i] == '\'') dst[j++] = '\'';
				}
				else dst[j++] = beg[i];
			}

			dst[j] = '\0';

			if (!dstName) {
				*units = (ConfigFormatUnit_t*)realloc(*units, sizeof(ConfigFormatUnit_t) * (*count + 1));
				if (!*units) continue;

				memcpy(&(*units)[*count], &tmpUnit, sizeof(ConfigFormatUnit_t));
				++*count;
				dstName = 1;
			}
		}
		else if (*ptr == '=') {
			if (dstName) dstName = 0;
			++ptr;
		}
		else if (isalnum(*ptr) || *ptr == '_') {
			beg = ptr;
			while (isalnum(*ptr) || *ptr == '_') ++ptr;
			end = ptr;

			tmpLen = (size_t)end - (size_t)beg;

			if (dstName) {
				tmpUnit.Name = (char*)malloc(tmpLen + 1);
				dst = (char*)&tmpUnit.Name[0];
				if (!dst) continue;
			}
			else {
				tmpUnit.Value = (char*)malloc(tmpLen + 1);
				dst = (char*)&tmpUnit.Value[0];
				if (!dst) {
					free(tmpUnit.Name);
					continue;
				}
			}

			for (i = 0, j = 0; i < tmpLen; i++) {
				if (beg[i] == '\\') {
					++i;
					if (beg[i] == '\\') dst[j++] = '\\';
					else if (beg[i] == '\"') dst[j++] = '\"';
					else if (beg[i] == '\'') dst[j++] = '\'';
				}
				else dst[j++] = beg[i];
			}

			dst[j] = '\0';

			if (!dstName) {
				*units = (ConfigFormatUnit_t*)realloc(*units, sizeof(ConfigFormatUnit_t) * (*count + 1));
				if (!*units) continue;

				memcpy(&(*units)[*count], &tmpUnit, sizeof(ConfigFormatUnit_t));
				++*count;
				dstName = 1;
			}
		}
		else ++ptr;
	}
}

void fmt_cfg_free_uniits(ConfigFormatUnit_t* units, size_t count) {
	for (size_t i = 0; i < count; i++) {
		if (units[i].Name) free(units[i].Name);
		if (units[i].Value) free(units[i].Value);
	}

	free(units);
}