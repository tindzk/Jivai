#ifndef STRING_ARRAY_H
#define STRING_ARRAY_H

#include "Array.h"

typedef struct {
	size_t len;
	size_t size;
	String *buf;
} StringArray;

#define StringArray_Init(...) \
	Array_Init(__VA_ARGS__)

#define StringArray_Reset(this, cnt) \
	Array_Reset(this, cnt, String_Destroy)

#define StringArray_Destroy(this) \
	Array_Destroy(this, String_Destroy)

#endif
