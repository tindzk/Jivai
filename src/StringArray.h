#ifndef STRING_ARRAY_H
#define STRING_ARRAY_H

#include "Array.h"
#include "String.h"

typedef struct {
	size_t len;
	size_t size;
	String *buf;
} StringArray;

#endif
