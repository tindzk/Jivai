#import "String.h"
#import "Integer.h"

#ifndef Number_BufferSize 
#define Number_BufferSize 30
#endif

void Number_FormatBuf(s32 number, size_t decimals, String *res);

#define Number_Format(num, dec) ({     \
	String _str = StackString(         \
		Number_BufferSize);            \
	Number_FormatBuf(num, dec, &_str); \
	_str; })
