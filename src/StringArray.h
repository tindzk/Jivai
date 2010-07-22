#ifndef STRING_ARRAY_H
#define STRING_ARRAY_H

#include "Array.h"

typedef struct {
	size_t len;
	size_t size;
	String buf[0];
} StringArray;

#endif
