#include "Hex.h"

static char symbols[] = "0123456789abcdef";

String Hex_ToString(long hex) {
	String res = HeapString(6);
	String_Append(&res, String("000000"));

	size_t digit;
	ssize_t i = res.len - 1;

	do {
		digit = hex % 16;
		res.buf[i] = symbols[digit];
		hex /= 16;
		i--;
	} while (i >= 0 && hex);

	return res;
}

long Hex_ToInteger(String s) {
	int  total      = 0;
	long multiplier = 1;

	ssize_t i = s.len - 1;
	size_t  j;

	do {
		for (j = 0; j < sizeof(symbols) - 1; j++) {
			if (Char_ToLower(s.buf[i]) == symbols[j]) {
				goto ok;
			}
		}

		return 0;

	ok:
		total += j * multiplier;
		multiplier *= 16;
		i--;
	} while (i >= 0);

	return total;
}
