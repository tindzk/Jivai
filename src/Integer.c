#import "Integer.h"

static ExceptionManager *exc;

void Integer0(ExceptionManager *e) {
	exc = e;
}

#define DefineParseString(type, name)           \
	type name(String s) {                       \
		if (s.len == 0) {                       \
			return 0;                           \
		}                                       \
		size_t i, j, c, digit;                  \
		type res;                               \
		c = 0;                                  \
		res = 0;                                \
		i = s.len;                              \
		while (i--) {                           \
			if (!Char_IsDigit(s.buf[i])) {      \
				continue;                       \
			}                                   \
			digit = Char_ParseDigit(s.buf[i]);  \
			j = c;                              \
			while (j--) {                       \
				digit *= 10;                    \
			}                                   \
			if (res + digit > MaxValue(type)) { \
				throw(exc, excNumberTooBig);    \
			}                                   \
			res += digit;                       \
			c++;                                \
		}                                       \
		if (s.buf[0] == '-') {                  \
			res *= -1;                          \
		}                                       \
		return res;                             \
	}

DefineParseString(s32, Integer_ParseString);
DefineParseString(s64, Integer64_ParseString);

/* To determine the number of digits. */
#define DefineCountDigits(type, name) \
	size_t name(type num) {           \
		size_t digits = 0;            \
		do {                          \
			num /= 10;                \
			digits++;                 \
		} while(num);                 \
		return digits;                \
	}

DefineCountDigits(s32, Integer_CountDigits);
DefineCountDigits(s64, Integer64_CountDigits);

#define DefineToStringBuf(type, name)                             \
	void name(type num, String *res) {                            \
		typeof(num) orig = num;                                   \
		size_t digit = res->size;                                 \
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
		} while(orig);                                            \
	}

DefineToStringBuf(s32, Integer_ToStringBuf);
DefineToStringBuf(s64, Integer64_ToStringBuf);

#define DefineCompare(type, name)       \
	inline short name(type a, type b) { \
		return (a > b)                  \
			 - (a < b);                 \
	}

DefineCompare(s32, Integer_Compare);
DefineCompare(s64, Integer64_Compare);
