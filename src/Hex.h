#ifndef HEX_H
#define HEX_H

#include "String.h"

String Hex_ToString(long hex);
int OVERLOAD Hex_ToInteger(char c);
long OVERLOAD Hex_ToInteger(String s);

#endif
