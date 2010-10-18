#import "Query.h"

size_t Modules_HTTP_Query;

static ExceptionManager *exc;

void HTTP_Query0(ExceptionManager *e) {
	Modules_HTTP_Query =
		Module_Register(String("HTTP.Query"));

	exc = e;
}

void HTTP_Query_Init(HTTP_Query *this, HTTP_OnParameter onParameter, void *context) {
	this->onParameter = onParameter;
	this->context     = context;
	this->autoResize  = false;
}

void HTTP_Query_SetAutoResize(HTTP_Query *this, bool value) {
	this->autoResize = value;
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

				if (high != -1 && low != -1) {
					*dst++ = (char)((high << 4) + low);
				}
			}

			i += 2;
		} else {
			*dst++ = src.buf[i];
		}
	}
}

void HTTP_Query_Decode(HTTP_Query *this, String s, bool isFormUri) {
	String name  = HeapString(0);
	size_t start = 0;

	for (size_t i = 0; i < s.len; i++) {
		if (s.buf[i] == '=') {
			String tmp = String_Slice(s, start, i - start);

			size_t len = HTTP_Query_GetAbsoluteLength(tmp);
			String_Align(&name, len);

			HTTP_Query_Unescape(tmp, name.buf, isFormUri);
			name.len = len;

			start = i;
		} else if (s.buf[i] == '&' || s.len == i + 1) {
			/* Handle malformed queries such as p1=v1&&&p2=v2. */
			if (i > 0 && s.buf[i - 1] == '&') {
				goto next;
			}

			String *value = this->onParameter(this->context, name);

			if (value == NULL) {
				/* Ignore parameter. */
				goto next;
			}

			String escaped;

			if (s.len == i + 1) {
				escaped = String_Slice(s,
					start + 1,
					i - start);
			} else {
				escaped = String_Slice(s,
					start + 1,
					i - start - 1);
			}

			size_t len = HTTP_Query_GetAbsoluteLength(escaped);

			if (len > value->size) {
				if (this->autoResize) {
					String_Resize(value, len);
				} else {
					String_Destroy(&name);
					throw(exc, excExceedsPermittedLength);
				}
			}

			HTTP_Query_Unescape(escaped, value->buf, isFormUri);
			value->len = len;

		next:
			start = i + 1;
		}
	}

	String_Destroy(&name);
}
