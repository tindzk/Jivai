#import <stdint.h>
#import <limits.h>

#import "String.h"
#import "ExceptionManager.h"

#undef self
#define self Integer

enum {
	excNumberTooBig = excOffset
};

void Integer0(ExceptionManager *e);

s32 Integer_ParseString(String s);
s64 Integer64_ParseString(String s);

size_t Integer_CountDigits(s32 num);
size_t Integer64_CountDigits(s64 num);

void Integer_ToStringBuf(s32 num, String *buf);
void Integer64_ToStringBuf(s64 num, String *buf);

short Integer_Compare(s32 a, s32 b);
short Integer64_Compare(s64 a, s64 b);

#define Integer_ToString(num) ({           \
	size_t len = Integer_CountDigits(num); \
	String str = StackString(len);         \
	Integer_ToStringBuf(num, &str);        \
	str; })

#define Integer64_ToString(num) ({           \
	size_t len = Integer64_CountDigits(num); \
	String str = StackString(len);           \
	Integer64_ToStringBuf(num, &str);        \
	str; })
