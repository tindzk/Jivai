#import "Charset.h"

/*
 * Portions Copyright (c) 1996-2009, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * Rendered after iso8859_1_to_utf8() from
 * src/backend/utils/mb/conversion_procs/utf8_and_iso8859_1/utf8_and_iso8859_1.c
 */
String Charset_Latin1ToUTF8(String s) {
	/* In worst case, the resulting string grows by factor 2.
	 * 1.2x appears to be a reasonable approximation.
	 */

	String res = String_New(s.len * 1.2);

	fwd(i, s.len) {
		unsigned short c = *(unsigned char *) &s.buf[i];

		if (Charset_IsHighBitSet(c)) {
			unsigned char a = (c >> 6) | 0xc0;
			unsigned char b = (c & 0x003f) | Charset_HighBit;

			String_Append(&res, a);
			String_Append(&res, b);
		} else {
			String_Append(&res, c);
		}
	}

	return res;
}
