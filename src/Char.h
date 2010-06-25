#ifndef CHAR_H
#define CHAR_H

#include "Boolean.h"
#include "UniStd.h" /* write() */

/* See also: http://php.net/manual/en/function.trim.php */
#define Char_IsSpace(c) \
	((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r' || (c) == '\0' || (c) == '\x0B')

#define Char_IsDigit(c) \
	(((c) >= '0') && ((c) <= '9'))

#define Char_ParseDigit(c) \
	(short) ((c) - 48)

/* Taken from diet libc (dietlibc-0.32/lib/tolower.c). */
#define Char_IsLower(c) \
	((size_t) ((c) - 'a') < 26u)

/* Taken from diet libc (dietlibc-0.32/lib/toupper.c). */
#define Char_IsUpper(c) \
	((size_t) ((c) - 'A') < 26u)

void Char_Print(char c);
bool Char_IsAlpha(int ch);
int Char_ToLower(int c);
int Char_ToUpper(int c);
bool Char_IsPrintable(int x);
bool Char_IsHexDigit(int ch);

#endif
