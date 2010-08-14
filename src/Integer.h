#ifndef INTEGER_H
#define INTEGER_H

#include <stdint.h>
#include <limits.h>

#include "ExceptionManager.h"
#include "String.h"

Exception_Export(Integer_NumberTooBigException);

void Integer0(ExceptionManager *e);

int Integer_ParseString(String s);
int64_t Integer64_ParseString(String s);

size_t Integer_CountDigits(int num);
size_t Integer64_CountDigits(int64_t num);

String Integer_ToStringBuf(int num, String buf);
String Integer64_ToStringBuf(int64_t num, String buf);

short Integer_Compare(int a, int b);
short Integer64_Compare(int64_t a, int64_t b);

#define Integer_ToString(num) \
	Integer_ToStringBuf(num, StackString(Integer_CountDigits(num) + (((num) < 0) ? 1 : 0)))

#define Integer64_ToString(num) \
	Integer64_ToStringBuf(num, StackString(Integer64_CountDigits(num) + (((num) < 0) ? 1 : 0)))

#endif
