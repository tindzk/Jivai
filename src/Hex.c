#include "Hex.h"

String Hex_ToString(int hex) {
	static char symbols[] = "0123456789abcdef";

	String res = HeapString(7);
	String_Append(&res, String("000000"));

	int i = res.len;
	size_t digit;

	do {
		digit = hex % 16;
		res.buf[i] = symbols[digit];
		hex /= 16;
		i--;
	} while (i >= 0 && hex);

	return res;
}
