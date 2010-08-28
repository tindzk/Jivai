#import <stdint.h>
#import <limits.h>

#import "String.h"
#import "ExceptionManager.h"

#undef self
#define self Integer

extern size_t Modules_Integer;

enum {
	excNumberTooBig = excOffset
};

void Integer0(ExceptionManager *e);

int Integer_ParseString(String s);
int64_t Integer64_ParseString(String s);

size_t Integer_CountDigits(int num);
size_t Integer64_CountDigits(int64_t num);

String Integer_ToStringBuf(int num, String buf);
String Integer64_ToStringBuf(int64_t num, String buf);

short Integer_Compare(int a, int b);
short Integer64_Compare(int64_t a, int64_t b);

#define Integer_Length   String("-2147483647").len
#define Integer64_Length String("-9223372036854775807").len

#define Integer_ToString(num) \
	Integer_ToStringBuf(num, StackString(Integer_Length))

#define Integer64_ToString(num) \
	Integer64_ToStringBuf(num, StackString(Integer64_Length))
