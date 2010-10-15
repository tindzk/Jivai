#import "Kernel.h"

/* See also: http://php.net/manual/en/function.trim.php */
#define Char_IsSpace(c) \
	((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r' || (c) == '\0' || (c) == '\x0B')

#define Char_IsDigit(c) \
	(((c) >= '0') && ((c) <= '9'))

#define Char_ParseDigit(c) \
	(short) ((c) - 48)

/* Taken from diet libc (dietlibc-0.32/lib/tolower.c). */
#define Char_IsLower(c) \
	((u32) ((c) - 'a') < 26u)

/* Taken from diet libc (dietlibc-0.32/lib/toupper.c). */
#define Char_IsUpper(c) \
	((u32) ((c) - 'A') < 26u)

void Char_Print(char c);
bool Char_IsAlpha(s32 ch);
s32 Char_ToLower(s32 ch);
s32 Char_ToUpper(s32 ch);
bool Char_IsPrintable(s32 ch);
bool Char_IsHexDigit(s32 ch);
