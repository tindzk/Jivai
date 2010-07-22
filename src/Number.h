#ifndef NUMBER_H
#define NUMBER_H

#include "String.h"

#ifndef Number_BufferSize 
#define Number_BufferSize 15
#endif

String Number_Format(float number, size_t decimals);

#endif
