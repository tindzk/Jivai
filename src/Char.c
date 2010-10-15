#import "Char.h"

void Char_Print(char c) {
	Kernel_write(FileNo_StdOut, &c, 1);
}

/* Taken from diet libc (dietlibc-0.32/lib/isalpha.c). */
bool Char_IsAlpha(s32 ch) {
	return (u32) ((ch | 0x20) - 'a') < 26u;
}

/* Taken from diet libc (dietlibc-0.32/lib/tolower.c). */
bool Char_IsLower(s32 ch) {
	return (u32) (ch - 'a') < 26u;
}

/* Taken from diet libc (dietlibc-0.32/lib/toupper.c). */
bool Char_IsUpper(s32 ch) {
	return (u32) (ch - 'A') < 26u;
}

/* Taken from diet libc (dietlibc-0.32/lib/tolower.c). */
s32 Char_ToLower(s32 ch) {
	if (Char_IsUpper(ch)) {
		ch += 'a' - 'A';
	}

	return ch;
}

/* Taken from diet libc (dietlibc-0.32/lib/toupper.c). */
s32 Char_ToUpper(s32 ch) {
	if (Char_IsLower(ch)) {
		ch += 'A' - 'a';
	}

	return ch;
}

/* Taken from diet libc (dietlibc-0.32/liblatin1/latin1-isprint.c). */
bool Char_IsPrintable(s32 ch) {
	u8 c = ch & 0xff;
	return (c >= 32 && c <= 126) || (c >= 160);
}

/* Taken from diet libc (dietlibc-0.32/lib/isxdigit.c). */
bool Char_IsHexDigit(s32 ch) {
	return (u32)( ch         - '0') < 10u
		|| (u32)((ch | 0x20) - 'a') <  6u;
}

/* See also: http://php.net/manual/en/function.trim.php */
bool Char_IsSpace(char c) {
	return c == ' '
		|| c == '\t'
		|| c == '\n'
		|| c == '\r'
		|| c == '\0'
		|| c == '\x0B';
}

bool Char_IsDigit(char c) {
	return c >= '0' && c <= '9';
}

short Char_ParseDigit(char c) {
	return (short) (c - 48);
}
