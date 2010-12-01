#import "String.h"

#define self Hex

sdef(String, ToString, u64 hex);
overload sdef(s8, ToInteger, char c);
overload sdef(s64, ToInteger, String s);

#undef self
