#import <stdint.h>

#import "UniStd.h"
#import "String.h"
#import "Compiler.h"

String Hex_ToString(uint64_t hex);
overload int Hex_ToInteger(char c);
overload long Hex_ToInteger(String s);
