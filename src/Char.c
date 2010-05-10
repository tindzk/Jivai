#include "Char.h"

void Char_Print(char c) {
	write(1, &c, 1);
}

/* Taken from diet libc (dietlibc-0.32/lib/tolower.c). */
char Char_ToLower(char c) {
	if (Char_IsUpper(c)) {
		c += 'a' - 'A';
	}

	return c;
}

/* Taken from diet libc (dietlibc-0.32/lib/toupper.c). */
char Char_ToUpper(char c) {
	if (Char_IsLower(c)) {
		c += 'A' - 'a';
	}

	return c;
}

/* Taken from diet libc (dietlibc-0.32/liblatin1/latin1-isprint.c). */
int Char_IsPrintable(int x) {
	unsigned char c = x & 0xff;
	return (c >= 32 && c <= 126) || (c >= 160);
}

/* Taken from diet libc (dietlibc-0.32/lib/isxdigit.c). */
bool Char_IsHexDigit(int ch) {
	return (size_t)( ch         - '0') < 10u
		|| (size_t)((ch | 0x20) - 'a') <  6u;
}
