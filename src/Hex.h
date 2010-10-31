#import <stdint.h>

#import "UniStd.h"
#import "String.h"
#import "Compiler.h"

#undef self
#define self Hex

sdef(String, ToString, u64 hex);
overload sdef(int, ToInteger, char c);
overload sdef(long, ToInteger, String s);
