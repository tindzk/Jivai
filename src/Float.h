#include <math.h>
#include <stdio.h>

#import "String.h"
#import "Integer.h"

void Float_ToStringBuf(float num, double precision, char sep, String *out);

#define Float_ToString(num, precision, sep) ({    \
	String str = StackString(80);                 \
	Float_ToStringBuf(num, precision, sep, &str); \
	str; })

float Float_Parse(String s, char sep);
bool Float_Equals(float f1, float f2, double precision);
