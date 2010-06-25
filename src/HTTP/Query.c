#include "Query.h"

Exception_Define(HTTP_Query_ExceedsPermittedLengthException);

static ExceptionManager *exc;

void HTTP_Query0(ExceptionManager *e) {
	exc = e;
}

void HTTP_Query_Init(HTTP_Query *this, HTTP_OnParameter onParameter, void *context) {
	this->onParameter = onParameter;
	this->context     = context;
}

size_t HTTP_Query_GetAbsoluteLength(String s) {
	size_t cnt = 0;

	for (size_t i = 0; i < s.len; i++) {
		if (s.buf[i] == '%' && i + 2 <= s.len) {
			if (Char_IsHexDigit(s.buf[i + 1])
			 && Char_IsHexDigit(s.buf[i + 2])) {
				cnt++;
			} else {
				cnt += 2;
			}

			i += 2;
		} else {
			cnt++;
		}
	}

	return cnt;
}

/* Adapted from GNet - Networking library
 * Copyright (C) 2000-2001  David Helder, David Bolcsfoldi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * Form-URI-encoded data differs from URI encoding in a way that it
 * uses '+' as character for space, see RFC 1866 section 8.2.1
 * http://ftp.ics.uci.edu/pub/ietf/html/rfc1866.txt
 */

void HTTP_Query_Unescape(String src, char *dst, bool isFormUri) {
	int high, low;

	for (size_t i = 0; i < src.len; i++) {
		if (isFormUri && src.buf[i] == '+') {
			*dst++ = ' ';
		} else if (src.buf[i] == '%' && i + 2 < src.len) {
			if (!Char_IsHexDigit(src.buf[i + 1])
			 || !Char_IsHexDigit(src.buf[i + 2])) {
				*dst++ = src.buf[i + 1];
				*dst++ = src.buf[i + 2];
			} else {
				high = Hex_ToInteger(src.buf[i + 1]);
				low  = Hex_ToInteger(src.buf[i + 2]);

				*dst++ = (char)((high << 4) + low);
			}

			i += 2;
		} else {
			*dst++ = src.buf[i];
		}
	}
}

void HTTP_Query_Decode(HTTP_Query *this, String s, bool isFormUri) {
	size_t len;
	size_t i = 0;
	size_t abslen;
	size_t start = 0;

	String tmp;
	String name = HeapString(0);

	while (i < s.len) {
		if (s.buf[i] == '=') {
			tmp.buf = s.buf + start;
			tmp.len = i - start;

			abslen = HTTP_Query_GetAbsoluteLength(tmp);
			String_Align(&name, abslen);

			HTTP_Query_Unescape(tmp, name.buf, isFormUri);
			name.len = abslen;

			start = i;
		} else if (s.buf[i] == '&' || s.len == i + 1) {
			/* Handle malformed queries such as p1=v1&&&p2=v2. */
			if ((i > 0) && (s.buf[i - 1] == '&')) {
				goto next;
			}

			String *value = this->onParameter(this->context, name);

			if (value == NULL) {
				/* Ignore parameter. */
				goto next;
			}

			if (s.len == i + 1) {
				len = i - start;
			} else {
				len = i - start - 1;
			}

			String val;
			val.buf = s.buf + start + 1;
			val.len = len;

			abslen = HTTP_Query_GetAbsoluteLength(val);

			if (abslen > value->size) {
				String_Destroy(&name);
				throw(exc, &HTTP_Query_ExceedsPermittedLengthException);
			}

			HTTP_Query_Unescape(val, value->buf, isFormUri);
			value->len = abslen;

		next:
			start = i + 1;
		}

		i++;
	}

	String_Destroy(&name);
}
