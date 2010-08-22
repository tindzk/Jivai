#import "Hex.h"

String Hex_ToString(uint64_t hex) {
	static char symbols[] = "0123456789abcdef";

	String res = HeapString(6);
	String_Append(&res, String("000000"));

	uint64_t digit;
	ssize_t i = res.len - 1;

	do {
		digit = hex % 16;
		res.buf[i] = symbols[digit];
		hex /= 16;
		i--;
	} while (i >= 0 && hex);

	return res;
}

overload int Hex_ToInteger(char c) {
	if ('a' <= c && c <= 'f') {
		return c - 'a' + 10;
	} else if ('A' <= c && c <= 'F') {
		return c - 'A' + 10;
	} else if ('0' <= c && c <= '9') {
		return c - '0';
	} else { /* The character is malformed. */
		return -1;
	}
}

overload long Hex_ToInteger(String s) {
	int  total      = 0;
	long multiplier = 1;

	ssize_t i = s.len - 1;

	do {
		int digit = Hex_ToInteger(s.buf[i]);

		if (digit == -1) {
			return -1;
		}

		total += digit * multiplier;
		multiplier *= 16;

		i--;
	} while (i >= 0);

	return total;
}
