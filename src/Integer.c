#import "Integer.h"

static ExceptionManager *exc;

void Integer0(ExceptionManager *e) {
	exc = e;
}

#define INTEGER_PARSE_STRING(s, type, max) \
	if (s.len == 0) {                      \
		return 0;                          \
	}                                      \
	size_t i, j, c, digit;                 \
	type res;                              \
	c = 0;                                 \
	res = 0;                               \
	i = s.len;                             \
	while (i--) {                          \
		if (!Char_IsDigit(s.buf[i])) {     \
			continue;                      \
		}                                  \
		digit = Char_ParseDigit(s.buf[i]); \
		j = c;                             \
		while (j--) {                      \
			digit *= 10;                   \
		}                                  \
		if (res + digit > max) {           \
			throw(exc, excNumberTooBig);   \
		}                                  \
		res += digit;                      \
		c++;                               \
	}                                      \
	if (s.buf[0] == '-') {                 \
		res *= -1;                         \
	}                                      \
	return res;

s32 Integer_ParseString(String s) {
	INTEGER_PARSE_STRING(s, s32, INT_MAX)
}

s64 Integer64_ParseString(String s) {
	INTEGER_PARSE_STRING(s, s64, INT64_MAX)
}

/* To determine the number of digits. */
#define INTEGER_COUNT_DIGITS(num) \
	size_t digits = 0;            \
	do {                          \
		num /= 10;                \
		digits++;                 \
	} while(num);                 \
	return digits;

size_t Integer_CountDigits(s32 num) {
	INTEGER_COUNT_DIGITS(num)
}

size_t Integer64_CountDigits(s64 num) {
	INTEGER_COUNT_DIGITS(num)
}

#define INTEGER_TO_STRING_BUF(num, res, digits)               \
	typeof(num) orig = num;                                   \
	size_t digit = digits;                                    \
	int offset;                                               \
	if (num < 0) {                                            \
		res->buf[0] = '-';                                    \
		offset = 1;                                           \
		orig *= -1;                                           \
	} else {                                                  \
		offset = 0;                                           \
	}                                                         \
	/* Force new length. */                                   \
	res->len = offset + digit;                                \
	/* Now operate on the original number. */                 \
	do {                                                      \
		/* Necessary as indexes start with 0. */              \
		digit--;                                              \
		/* Convert current ones digit (48 = (int) '0'). */    \
		res->buf[offset + digit] = (char) ((orig % 10) + 48); \
		/* Remove current ones digit. */                      \
		orig /= 10;                                           \
	} while(orig);

void Integer_ToStringBuf(s32 num, String *res) {
	INTEGER_TO_STRING_BUF(num, res, res->size)
}

void Integer64_ToStringBuf(s64 num, String *res) {
	INTEGER_TO_STRING_BUF(num, res, res->size)
}

inline short Integer_Compare(s32 a, s32 b) {
	return (a > b)
		 - (a < b);
}

inline short Integer64_Compare(s64 a, s64 b) {
	return (a > b)
		 - (a < b);
}
