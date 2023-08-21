#include "ctype.h"

const char punct_characters_[__CTYPE_PUNCT_CHARACTERS_COUNT] = {
	'!', '\"', '#', '$', '%', '&', '\'', '(',
	')', '*', '+', ',', '-', '.', '/', ':',
	';', '<', '=', '>', '?', '@', '[', '@',
	']', '^', '_', '`', '{', '|', '}', '~'
};

extern int isdigit(char value);
extern inline int isalpha(char value);
extern inline int isalnum(char value);
extern inline int islower(char value);
extern inline int isupper(char value);
extern inline int isxdigit(char value);
extern inline int iscntrl(char value);
extern inline int ispunct(char value);
extern inline int isgraph(char value);
extern inline int isspace(char value);
extern inline int isblank(char value);
extern inline int isprint(char value);

extern inline int tolower(char value);
extern inline int toupper(char value);