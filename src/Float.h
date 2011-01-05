#include <math.h>
#include <stdio.h>

#import "String.h"
#import "Integer.h"

String Float_ToString(float num, double precision, char sep);
float Float_Parse(String s, char sep);
bool Float_Equals(float f1, float f2, double precision);
