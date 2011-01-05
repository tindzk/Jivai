#import "String.h"
#import "Exception.h"

#define self Integer

// @exc Overflow
// @exc Underflow

s8  Int8_Parse(String s);
s16 Int16_Parse(String s);
s32 Int32_Parse(String s);
s64 Int64_Parse(String s);

u8  UInt8_Parse(String s);
u16 UInt16_Parse(String s);
u32 UInt32_Parse(String s);
u64 UInt64_Parse(String s);

size_t Int8_CountDigits(s8 num);
size_t Int16_CountDigits(s16 num);
size_t Int32_CountDigits(s32 num);
size_t Int64_CountDigits(s64 num);

static overload alwaysInline size_t Integer_CountDigits(s8 num) {
	return Int8_CountDigits(num);
}

static overload alwaysInline size_t Integer_CountDigits(s16 num) {
	return Int16_CountDigits(num);
}

static overload alwaysInline size_t Integer_CountDigits(s32 num) {
	return Int32_CountDigits(num);
}

static overload alwaysInline size_t Integer_CountDigits(s64 num) {
	return Int64_CountDigits(num);
}

#if defined(__x86_64__)
static overload alwaysInline size_t Integer_CountDigits(ssize_t num) {
	return Int64_CountDigits(num);
}
#endif

size_t UInt8_CountDigits(u8 num);
size_t UInt16_CountDigits(u16 num);
size_t UInt32_CountDigits(u32 num);
size_t UInt64_CountDigits(u64 num);

static overload alwaysInline size_t Integer_CountDigits(u8 num) {
	return UInt8_CountDigits(num);
}

static overload alwaysInline size_t Integer_CountDigits(u16 num) {
	return UInt16_CountDigits(num);
}

static overload alwaysInline size_t Integer_CountDigits(u32 num) {
	return UInt32_CountDigits(num);
}

static overload alwaysInline size_t Integer_CountDigits(u64 num) {
	return UInt64_CountDigits(num);
}

#if defined(__x86_64__)
static overload alwaysInline size_t Integer_CountDigits(size_t num) {
	return UInt64_CountDigits(num);
}
#endif

String Int8_ToString(s8 num);
String Int16_ToString(s16 num);
String Int32_ToString(s32 num);
String Int64_ToString(s64 num);

static overload alwaysInline String Integer_ToString(s8 num) {
	return Int8_ToString(num);
}

static overload alwaysInline String Integer_ToString(s16 num) {
	return Int16_ToString(num);
}

static overload alwaysInline String Integer_ToString(s32 num) {
	return Int32_ToString(num);
}

static overload alwaysInline String Integer_ToString(s64 num) {
	return Int64_ToString(num);
}

#if defined(__x86_64__)
static overload alwaysInline String Integer_ToString(ssize_t num) {
	return Int64_ToString(num);
}
#endif

String UInt8_ToString(u8 num);
String UInt16_ToString(u16 num);
String UInt32_ToString(u32 num);
String UInt64_ToString(u64 num);

static overload alwaysInline String Integer_ToString(u8 num) {
	return UInt8_ToString(num);
}

static overload alwaysInline String Integer_ToString(u16 num) {
	return UInt16_ToString(num);
}

static overload alwaysInline String Integer_ToString(u32 num) {
	return UInt32_ToString(num);
}

static overload alwaysInline String Integer_ToString(u64 num) {
	return UInt64_ToString(num);
}

#if defined(__x86_64__)
static overload alwaysInline String Integer_ToString(size_t num) {
	return UInt64_ToString(num);
}
#endif

short Int8_Compare(s8 a, s8 b);
short Int16_Compare(s16 a, s16 b);
short Int32_Compare(s32 a, s32 b);
short Int64_Compare(s64 a, s64 b);

static overload alwaysInline short Integer_Compare(s8 a, s8 b) {
	return Int8_Compare(a, b);
}

static overload alwaysInline short Integer_Compare(s16 a, s16 b) {
	return Int16_Compare(a, b);
}

static overload alwaysInline short Integer_Compare(s32 a, s32 b) {
	return Int32_Compare(a, b);
}

static overload alwaysInline short Integer_Compare(s64 a, s64 b) {
	return Int64_Compare(a, b);
}

#if defined(__x86_64__)
static overload alwaysInline short Integer_Compare(ssize_t a, ssize_t b) {
	return Int64_Compare(a, b);
}
#endif

short UInt8_Compare(u8 a, u8 b);
short UInt16_Compare(u16 a, u16 b);
short UInt32_Compare(u32 a, u32 b);
short UInt64_Compare(u64 a, u64 b);

static overload alwaysInline short Integer_Compare(u8 a, u8 b) {
	return UInt8_Compare(a, b);
}

static overload alwaysInline short Integer_Compare(u16 a, u16 b) {
	return UInt16_Compare(a, b);
}

static overload alwaysInline short Integer_Compare(u32 a, u32 b) {
	return UInt32_Compare(a, b);
}

static overload alwaysInline short Integer_Compare(u64 a, u64 b) {
	return UInt64_Compare(a, b);
}

#if defined(__x86_64__)
static overload alwaysInline short Integer_Compare(size_t a, size_t b) {
	return UInt64_Compare(a, b);
}
#endif

#undef self
