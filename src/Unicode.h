#ifndef UNICODE_H
#define UNICODE_H

#include "String.h"

size_t Unicode_CalcWidth(const char *src);
size_t Unicode_Next(String s, size_t offset);
size_t Unicode_Prev(String s, size_t offset);
size_t Unicode_Count(String s);

#endif
