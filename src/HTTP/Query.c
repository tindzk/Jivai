#import "Query.h"

#define self HTTP_Query

def(void, Init, HTTP_OnParameter onParameter) {
	this->onParameter = onParameter;
	this->autoResize  = false;
}

def(void, SetAutoResize, bool value) {
	this->autoResize = value;
}

sdef(size_t, GetAbsoluteLength, String s) {
	size_t cnt = 0;

	for (size_t i = 0; i < s.len; i++) {
		if (s.buf[i] == '%' && i + 2 < s.len) {
			if (Char_IsHexDigit(s.buf[i + 1])
			 && Char_IsHexDigit(s.buf[i + 2])) {
				cnt++;
			} else {
				cnt += 3;
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

sdef(void, Unescape, String src, char *dst, bool isFormUri) {
	int high, low;

	for (size_t i = 0; i < src.len; i++) {
		if (isFormUri && src.buf[i] == '+') {
			*dst++ = ' ';
		} else if (src.buf[i] == '%' && i + 2 < src.len) {
			if (!Char_IsHexDigit(src.buf[i + 1])
			 || !Char_IsHexDigit(src.buf[i + 2])) {
				*dst++ = src.buf[i];
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

static String _reserved[] = {
	['!']  = $("%21"),
	['*']  = $("%2A"),
	['\''] = $("%27"),
	['(']  = $("%28"),
	[')']  = $("%29"),
	[';']  = $("%3B"),
	[':']  = $("%3A"),
	['@']  = $("%40"),
	['&']  = $("%26"),
	['=']  = $("%3D"),
	['+']  = $("%2B"),
	['$']  = $("%24"),
	[',']  = $("%2C"),
	['/']  = $("%2F"),
	['?']  = $("%3F"),
	['#']  = $("%23"),
	['[']  = $("%5B"),
	[']']  = $("%5D"),
	[' ']  = $("+")
};

sdef(String, Encode, String param) {
	String res = String_New(param.len * 1.3);

	forward (i, param.len) {
		size_t c = param.buf[i];

		if (c < nElems(_reserved) && _reserved[c].len != 0) {
			String_Append(&res, _reserved[c]);
		} else {
			String_Append(&res, (char) c);
		}
	}

	return res;
}

def(void, Decode, String s, bool isFormUri) {
	String name  = $("");
	size_t start = 0;

	forward (i, s.len) {
		if (s.buf[i] == '=') {
			String tmp = String_Slice(s, start, i - start);

			size_t len = scall(GetAbsoluteLength, tmp);
			String_Align(&name, len);

			scall(Unescape, tmp, name.buf, isFormUri);
			name.len = len;

			start = i + 1;
		} else if (s.buf[i] == '&' || s.len == i + 1) {
			if (s.len == i + 1) {
				if (s.buf[i] != '&') {
					i++;
				}
			}

			String escaped = String_Slice(s, start, i - start);

			if (escaped.len == 0) {
				goto next;
			}

			/* Parameter is an option. */
			if (name.len == 0) {
				String *value = callbackRet(this->onParameter, NULL, escaped);

				if (value != NULL) {
					value->len = 0;
				}

				goto next;
			}

			String *value = callbackRet(this->onParameter, NULL, name);

			if (value == NULL) {
				/* Ignore parameter. */
				goto next;
			}

			size_t len = scall(GetAbsoluteLength, escaped);

			if (len > String_GetSize(value)) {
				if (this->autoResize) {
					String_Resize(value, len);
				} else {
					String_Destroy(&name);
					throw(ExceedsPermittedLength);
				}
			}

			scall(Unescape, escaped, value->buf, isFormUri);
			value->len = len;

		next:
			start    = i + 1;
			name.len = 0;
		}
	}

	String_Destroy(&name);
}
