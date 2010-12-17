#import "Float.h"

/* Taken from
 * http://stackoverflow.com/questions/2302969/how-to-implement-char-ftoafloat-num-without-sprintf-library-function
 */
void Float_ToStringBuf(float num, double precision, String *out) {
	int m = (int) log10(num);

	while (m >= 0 || num > 0 + precision) {
		double weight = pow(10.0f, m);
		int digit     = (int) floor(num / weight);

		num -= digit * weight;

		String_Append(out, '0' + (char) digit);

		if (m == 0) {
			String_Append(out, '.');
		}

		m--;
	}

	/* Add missing zeros. */
	ssize_t digits = 0;

	for (; precision > 0 && precision < 1; digits++) {
		precision *= 10;
	}

	digits -= m * -1;

	for (; digits >= 0; digits--) {
		String_Append(out, '0');
	}
}
