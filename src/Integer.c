#include "Integer.h"

Exception_Define(Integer_NumberTooBigException);

static ExceptionManager *exc;

void Integer0(ExceptionManager *e) {
	exc = e;
}

#define INTEGER_PARSE_STRING(s, type, max)              \
	if (s.len == 0) {                                   \
		return 0;                                       \
	}                                                   \
	size_t i, j, c, digit;                              \
	type res;                                           \
	c = 0;                                              \
	res = 0;                                            \
	i = s.len;                                          \
	while (i--) {                                       \
		if (!Char_IsDigit(s.buf[i])) {                  \
			continue;                                   \
		}                                               \
		digit = Char_ParseDigit(s.buf[i]);              \
		j = c;                                          \
		while (j--) {                                   \
			digit *= 10;                                \
		}                                               \
		if (res + digit > max) {                        \
			throw(exc, &Integer_NumberTooBigException); \
		}                                               \
		res += digit;                                   \
		c++;                                            \
	}                                                   \
	if (s.buf[0] == '-') {                              \
		res *= -1;                                      \
	}                                                   \
	return res;

int Integer_ParseString(String s) {
	INTEGER_PARSE_STRING(s, int, INT_MAX)
}

int64_t Integer64_ParseString(String s) {
	INTEGER_PARSE_STRING(s, int64_t, INT64_MAX)
}

/* To determine the number of digits. */
#define INTEGER_COUNT_DIGITS(num) \
	size_t digits = 0;            \
	do {                          \
		num /= 10;                \
		digits++;                 \
	} while(num);                 \
	return digits;

size_t Integer_CountDigits(int num) {
	INTEGER_COUNT_DIGITS(num)
}

size_t Integer64_CountDigits(int64_t num) {
	INTEGER_COUNT_DIGITS(num)
}

#define INTEGER_TO_STRING_BUF(num, res, digits, type)        \
	type orig = num;                                         \
	size_t digit = digits;                                   \
	int offset;                                              \
	if (num < 0) {                                           \
		res.buf[0] = '-';                                    \
		offset = 1;                                          \
		orig *= -1;                                          \
	} else {                                                 \
		offset = 0;                                          \
	}                                                        \
	/* Force new length. */                                  \
	res.len = offset + digit;                                \
	/* Now operate on the original number. */                \
	do {                                                     \
		/* Necessary as indexes start with 0. */             \
		digit--;                                             \
		/* Convert current ones digit (48 = (int) '0'). */   \
		res.buf[offset + digit] = (char) ((orig % 10) + 48); \
		/* Remove current ones digit. */                     \
		orig /= 10;                                          \
	} while(orig);                                           \
	return res;

String Integer_ToStringBuf(int num, String res) {
	INTEGER_TO_STRING_BUF(num, res, Integer_CountDigits(num), int)
}

String Integer64_ToStringBuf(int64_t num, String res) {
	INTEGER_TO_STRING_BUF(num, res, Integer64_CountDigits(num), int64_t)
}

int Integer_Compare(int a, int b) {
	return (a > b)
		 - (a < b);
}

int Integer64_Compare(int64_t a, int64_t b) {
	return (a > b)
		 - (a < b);
}
