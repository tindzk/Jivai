#import "Number.h"

String Number_Format(s32 number, size_t decimals) {
	String res = String_New(Number_BufferSize);

	bool isNeg = (number < 0);

	if (isNeg) {
		number *= -1;
	}

	/* n = 10^(decimals-1) */
	int n = 1;
	repeat (decimals - 1) {
		n *= 10;
	}

	if (isNeg) {
		String_Append(&res, '-');
	}

	while (number < n && n > 1) {
		String_Append(&res, '0');
		n /= 10;
	}

	String strNumber = Integer_ToString(number);
	String_Append(&res, strNumber.rd);
	String_Destroy(&strNumber);

	return res;
}
