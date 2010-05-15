#ifndef UNICODE_H
#define UNICODE_H

#include "String.h"

size_t Unicode_CalcWidth(const char *src);
size_t Unicode_Next(String s, size_t offset);
size_t Unicode_Prev(String s, size_t offset);
size_t Unicode_CountRange(String s, size_t offset, size_t len);

#define Unicode_Count(s) \
	Unicode_CountRange(s, 0, (s).len)

#endif
