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

float Float_Parse(String s, char sep) {
	String sdigit   = $("");
	String sdecimal = $("");

	forward (i, s.len) {
		if (s.buf[i] == sep) {
			sdigit   = String_Slice(s, 0, i);
			sdecimal = String_Slice(s, i + 1);

			break;
		}
	}

	s32 digit     = Int32_Parse(sdigit);
	float decimal = Int32_Parse(sdecimal);
	size_t digits = Integer_CountDigits((s32) decimal);

	repeat (digits) {
		decimal /= 10;
	}

	return (digit < 0)
		? digit - decimal
		: digit + decimal;
}

bool Float_Equals(float f1, float f2, double precision) {
	return f1 - precision < f2
		&& f2 + precision > f2;
}
