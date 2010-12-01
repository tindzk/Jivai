#import "String.h"
#import "Exception.h"

#define self Integer

enum {
	excOverflow = excOffset,
	excUnderflow
};

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

size_t UInt8_CountDigits(u8 num);
size_t UInt16_CountDigits(u16 num);
size_t UInt32_CountDigits(u32 num);
size_t UInt64_CountDigits(u64 num);

static overload inline size_t Integer_CountDigits(u8 num) {
	return UInt8_CountDigits(num);
}

static overload inline size_t Integer_CountDigits(u16 num) {
	return UInt16_CountDigits(num);
}

static overload inline size_t Integer_CountDigits(u32 num) {
	return UInt32_CountDigits(num);
}

static overload inline size_t Integer_CountDigits(u64 num) {
	return UInt64_CountDigits(num);
}

void Int8_ToStringBuf(s8 num, String *buf);
void Int16_ToStringBuf(s16 num, String *buf);
void Int32_ToStringBuf(s32 num, String *buf);
void Int64_ToStringBuf(s64 num, String *buf);

static overload inline void Integer_ToStringBuf(s8 num, String *buf) {
	Int8_ToStringBuf(num, buf);
}

static overload inline void Integer_ToStringBuf(s16 num, String *buf) {
	Int16_ToStringBuf(num, buf);
}

static overload inline void Integer_ToStringBuf(s32 num, String *buf) {
	Int32_ToStringBuf(num, buf);
}

static overload inline void Integer_ToStringBuf(s64 num, String *buf) {
	Int64_ToStringBuf(num, buf);
}

void UInt8_ToStringBuf(u8 num, String *buf);
void UInt16_ToStringBuf(u16 num, String *buf);
void UInt32_ToStringBuf(u32 num, String *buf);
void UInt64_ToStringBuf(u64 num, String *buf);

static overload inline void Integer_ToStringBuf(u8 num, String *buf) {
	UInt8_ToStringBuf(num, buf);
}

static overload inline void Integer_ToStringBuf(u16 num, String *buf) {
	UInt16_ToStringBuf(num, buf);
}

static overload inline void Integer_ToStringBuf(u32 num, String *buf) {
	UInt32_ToStringBuf(num, buf);
}

static overload inline void Integer_ToStringBuf(u64 num, String *buf) {
	UInt64_ToStringBuf(num, buf);
}

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

short UInt8_Compare(u8 a, u8 b);
short UInt16_Compare(u16 a, u16 b);
short UInt32_Compare(u32 a, u32 b);
short UInt64_Compare(u64 a, u64 b);

static overload inline short Integer_Compare(u8 a, u8 b) {
	return UInt8_Compare(a, b);
}

static overload inline short Integer_Compare(u16 a, u16 b) {
	return UInt16_Compare(a, b);
}

static overload inline short Integer_Compare(u32 a, u32 b) {
	return UInt32_Compare(a, b);
}

static overload inline short Integer_Compare(u64 a, u64 b) {
	return UInt64_Compare(a, b);
}

#define Int8_ToString(num) ({           \
	size_t len = Int8_CountDigits(num); \
	String _str = StackString(len);     \
	Int8_ToStringBuf(num, &_str);       \
	_str; })

#define Int16_ToString(num) ({           \
	size_t len = Int16_CountDigits(num); \
	String _str = StackString(len);      \
	Int16_ToStringBuf(num, &_str);       \
	_str; })

#define Int32_ToString(num) ({           \
	size_t len = Int32_CountDigits(num); \
	String _str = StackString(len);      \
	Int32_ToStringBuf(num, &_str);       \
	_str; })

#define Int64_ToString(num) ({           \
	size_t len = Int64_CountDigits(num); \
	String _str = StackString(len);      \
	Int64_ToStringBuf(num, &_str);       \
	_str; })

#define UInt8_ToString(num) ({           \
	size_t len = UInt8_CountDigits(num); \
	String _str = StackString(len);      \
	UInt8_ToStringBuf(num, &_str);       \
	_str; })

#define UInt16_ToString(num) ({           \
	size_t len = UInt16_CountDigits(num); \
	String _str = StackString(len);       \
	UInt16_ToStringBuf(num, &_str);       \
	_str; })

#define UInt32_ToString(num) ({           \
	size_t len = UInt32_CountDigits(num); \
	String _str = StackString(len);       \
	UInt32_ToStringBuf(num, &_str);       \
	_str; })

#define UInt64_ToString(num) ({           \
	size_t len = UInt64_CountDigits(num); \
	String _str = StackString(len);       \
	UInt64_ToStringBuf(num, &_str);       \
	_str; })

#define Integer_ToString(num) ({           \
	size_t len = Integer_CountDigits(num); \
	String _str = StackString(len);        \
	Integer_ToStringBuf(num, &_str);       \
	_str; })

#undef self
