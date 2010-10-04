#include <math.h>
#include <stdio.h>

#import "String.h"

void Float_ToStringBuf(float num, double precision, String *out);

#define Float_ToString(num, precision) ({    \
	String str = StackString(80);            \
	Float_ToStringBuf(num, precision, &str); \
	str; })
