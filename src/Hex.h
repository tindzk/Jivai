#import "String.h"

#undef self
#define self Hex

sdef(String, ToString, u64 hex);
overload sdef(int, ToInteger, char c);
overload sdef(long, ToInteger, String s);
