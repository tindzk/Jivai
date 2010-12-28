#import "Number.h"

void Number_FormatBuf(s32 number, size_t decimals, String *res) {
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
		String_Append(res, '-');
	}

	while (number < n && n > 1) {
		String_Append(res, '0');
		n /= 10;
	}

	String_Append(res, Int32_ToString(number));
}
