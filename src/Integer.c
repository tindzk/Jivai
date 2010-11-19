#import "Integer.h"

static ExceptionManager *exc;

void Integer0(ExceptionManager *e) {
	exc = e;
}

#define DefineParseString(type, name)                 \
	type name(String s) {                             \
		type res = 0;                                 \
		size_t c = 0;                                 \
		reverse (i, s.len) {                          \
			if (!Char_IsDigit(s.buf[i])) {            \
				continue;                             \
			}                                         \
			size_t digit = Char_ParseDigit(s.buf[i]); \
			repeat (c) {                              \
				digit *= 10;                          \
			}                                         \
			if (res + digit > MaxValue(type)) {       \
				throw(exc, excNumberTooBig);          \
			}                                         \
			res += digit;                             \
			c++;                                      \
		}                                             \
		if (s.len > 0 && s.buf[0] == '-') {           \
			res *= -1;                                \
		}                                             \
		return res;                                   \
	}

DefineParseString(s8,  Int8_Parse);
DefineParseString(s16, Int16_Parse);
DefineParseString(s32, Int32_Parse);
DefineParseString(s64, Int64_Parse);

DefineParseString(u8,  UInt8_Parse);
DefineParseString(u16, UInt16_Parse);
DefineParseString(u32, UInt32_Parse);
DefineParseString(u64, UInt64_Parse);

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

DefineCountDigits(s8,  Int8_CountDigits);
DefineCountDigits(s16, Int16_CountDigits);
DefineCountDigits(s32, Int32_CountDigits);
DefineCountDigits(s64, Int64_CountDigits);

DefineCountDigits(u8,  UInt8_CountDigits);
DefineCountDigits(u16, UInt16_CountDigits);
DefineCountDigits(u32, UInt32_CountDigits);
DefineCountDigits(u64, UInt64_CountDigits);

#define DefineToStringBuf(type, name)                             \
	void name(type num, String *res) {                            \
		typeof(num) orig = num;                                   \
		size_t digit = res->size;                                 \
		u8 offset;                                                \
		if (num == 0 || num > 0) {                                \
			offset = 0;                                           \
		} else {                                                  \
			res->buf[0] = '-';                                    \
			offset = 1;                                           \
			orig *= -1;                                           \
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

DefineToStringBuf(s8,  Int8_ToStringBuf);
DefineToStringBuf(s16, Int16_ToStringBuf);
DefineToStringBuf(s32, Int32_ToStringBuf);
DefineToStringBuf(s64, Int64_ToStringBuf);

DefineToStringBuf(u8,  UInt8_ToStringBuf);
DefineToStringBuf(u16, UInt16_ToStringBuf);
DefineToStringBuf(u32, UInt32_ToStringBuf);
DefineToStringBuf(u64, UInt64_ToStringBuf);

#define DefineCompare(type, name)       \
	inline short name(type a, type b) { \
		return (a > b)                  \
			 - (a < b);                 \
	}

DefineCompare(s8,  Int8_Compare);
DefineCompare(s16, Int16_Compare);
DefineCompare(s32, Int32_Compare);
DefineCompare(s64, Int64_Compare);

DefineCompare(u8,  UInt8_Compare);
DefineCompare(u16, UInt16_Compare);
DefineCompare(u32, UInt32_Compare);
DefineCompare(u64, UInt64_Compare);
