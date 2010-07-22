#include "Number.h"

String Number_Format(float number, size_t decimals) {
	bool isNeg = (number < 0);

	if (isNeg) {
		number *= -1;
	}

	/* n = 10^(decimals-1) */
	int n = 1;
	while (decimals > 1) {
		n *= 10;
		decimals--;
	}

	String res = HeapString(Number_BufferSize);

	if (isNeg) {
		String_Append(&res, '-');
	}

	while (number < n && n > 1) {
		String_Append(&res, '0');
		n /= 10;
	}

	String_Append(&res, Integer_ToString(number));

	return res;
}
