#import <stdint.h>

#import "UniStd.h"
#import "String.h"
#import "Compiler.h"

String Hex_ToString(u64 hex);
overload int Hex_ToInteger(char c);
overload long Hex_ToInteger(String s);
