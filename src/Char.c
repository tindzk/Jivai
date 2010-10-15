#import "Char.h"

void Char_Print(char c) {
	Kernel_write(FileNo_StdOut, &c, 1);
}

/* Taken from diet libc (dietlibc-0.32/lib/isalpha.c). */
bool Char_IsAlpha(int ch) {
	return (unsigned int) ((ch | 0x20) - 'a') < 26u;
}

/* Taken from diet libc (dietlibc-0.32/lib/tolower.c). */
int Char_ToLower(int c) {
	if (Char_IsUpper(c)) {
		c += 'a' - 'A';
	}

	return c;
}

/* Taken from diet libc (dietlibc-0.32/lib/toupper.c). */
int Char_ToUpper(int c) {
	if (Char_IsLower(c)) {
		c += 'A' - 'a';
	}

	return c;
}

/* Taken from diet libc (dietlibc-0.32/liblatin1/latin1-isprint.c). */
bool Char_IsPrintable(int x) {
	unsigned char c = x & 0xff;
	return (c >= 32 && c <= 126) || (c >= 160);
}

/* Taken from diet libc (dietlibc-0.32/lib/isxdigit.c). */
bool Char_IsHexDigit(int ch) {
	return (size_t)( ch         - '0') < 10u
		|| (size_t)((ch | 0x20) - 'a') <  6u;
}
