#import "String.h"
#import "Exception.h"

#undef self
#define self Integer

enum {
	excNumberTooBig = excOffset
};

void Integer0(ExceptionManager *e);

s8  Int8_Parse(String s);
s16 Int16_Parse(String s);
s32 Int32_Parse(String s);
s64 Int64_Parse(String s);

size_t Int8_CountDigits(s8 num);
size_t Int16_CountDigits(s16 num);
size_t Int32_CountDigits(s32 num);
size_t Int64_CountDigits(s64 num);

static overload inline size_t Integer_CountDigits(s8 num) {
	return Int8_CountDigits(num);
}

static overload inline size_t Integer_CountDigits(s16 num) {
	return Int16_CountDigits(num);
}

static overload inline size_t Integer_CountDigits(s32 num) {
	return Int32_CountDigits(num);
}

static overload inline size_t Integer_CountDigits(s64 num) {
	return Int64_CountDigits(num);
}

void Int8_ToStringBuf(s8 num, String *buf);
void Int16_ToStringBuf(s16 num, String *buf);
void Int32_ToStringBuf(s32 num, String *buf);
void Int64_ToStringBuf(s64 num, String *buf);

short Int8_Compare(s8 a, s8 b);
short Int16_Compare(s16 a, s16 b);
short Int32_Compare(s32 a, s32 b);
short Int64_Compare(s64 a, s64 b);

static overload inline short Integer_Compare(s8 a, s8 b) {
	return Int8_Compare(a, b);
}

static overload inline short Integer_Compare(s16 a, s16 b) {
	return Int16_Compare(a, b);
}

static overload inline short Integer_Compare(s32 a, s32 b) {
	return Int32_Compare(a, b);
}

static overload inline short Integer_Compare(s64 a, s64 b) {
	return Int64_Compare(a, b);
}

#define Int8_ToString(num) ({           \
	size_t len = Int8_CountDigits(num); \
	String str = StackString(len);      \
	Int8_ToStringBuf(num, &str);        \
	str; })

#define Int16_ToString(num) ({           \
	size_t len = Int16_CountDigits(num); \
	String str = StackString(len);       \
	Int16_ToStringBuf(num, &str);        \
	str; })

#define Int32_ToString(num) ({           \
	size_t len = Int32_CountDigits(num); \
	String str = StackString(len);       \
	Int32_ToStringBuf(num, &str);        \
	str; })

#define Int64_ToString(num) ({           \
	size_t len = Int64_CountDigits(num); \
	String str = StackString(len);       \
	Int64_ToStringBuf(num, &str);        \
	str; })

#define Integer_ToString(num) ({           \
	size_t len = Integer_CountDigits(num); \
	String str = StackString(len);         \
	Integer_ToStringBuf(num, &str);        \
	str; })
