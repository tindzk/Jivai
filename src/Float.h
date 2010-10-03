#include <math.h>
#include <stdio.h>

#import "String.h"

String Float_ToStringBuf(float num, double precision, String out);

#define Float_ToString(num, precision) \
	Float_ToStringBuf(num, precision, StackString(80))
