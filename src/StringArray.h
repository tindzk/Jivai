#ifndef STRING_ARRAY_H
#define STRING_ARRAY_H

#include "Array.h"

typedef struct {
	size_t len;
	size_t size;
	String buf[0];
} StringArray;

#define StringArray_Init(this, len) \
	Array_Init(this, len)

#define StringArray_Push(this, s) \
	Array_Push(this, String_Clone(s));

#define StringArray_Destroy(this)            \
	do {                                     \
		Array_Foreach(this, String_Destroy); \
		Array_Destroy(this);                 \
	} while(0)

#endif
