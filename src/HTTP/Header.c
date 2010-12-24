#import "Header.h"

#define self HTTP_Header

def(void, Init, ref(Events) events) {
	this->events = events;
}

def(void, ParseMethod, String s) {
	if (s.len == 0) {
		throw(RequestMalformed);
	}

	HTTP_Method method = HTTP_Method_FromString(s);

	if (method == HTTP_Method_Unset) {
		throw(UnknownMethod);
	}

	callback(this->events.onMethod, method);
}

def(void, ParseVersion, String s) {
	HTTP_Version version = HTTP_Version_FromString(s);

	if (version == HTTP_Version_Unset) {
		throw(UnknownVersion);
	}

	callback(this->events.onVersion, version);
}

def(void, ParseStatus, String s) {
	s32 code = Int32_Parse(s);

	if (code == 0) {
		throw(UnknownStatus);
	}

	HTTP_Status status = HTTP_Status_GetStatusByCode(code);

	if (status == HTTP_Status_Unset) {
		throw(UnknownStatus);
	}

	callback(this->events.onStatus, status);
}

def(void, ParseUri, String s) {
	if (s.len == 0) {
		throw(EmptyRequestUri);
	}

	ssize_t pos = String_Find(s, '?');

	if (hasCallback(this->events.onPath)) {
		String path =
			(pos != String_NotFound)
				? String_Slice(s, 0, pos)
				: s;

		size_t len = HTTP_Query_GetAbsoluteLength(path);

		if (len <= path.len) {
			HTTP_Query_Unescape(path, path.buf, true);
			path.len = len;
		} else {
			String decoded = HeapString(len);
			HTTP_Query_Unescape(path, decoded.buf, true);
			decoded.len = len;

			path = decoded;
		}

		try {
			callback(this->events.onPath, path);
		} clean finally {
			if (path.mutable) {
				String_Destroy(&path);
			}
		} tryEnd;
	}

	if (hasCallback(this->events.onParameter)) {
		if (pos != String_NotFound) {
			HTTP_Query qry;
			HTTP_Query_Init(&qry, this->events.onParameter);
			HTTP_Query_SetAutoResize(&qry, true);
			HTTP_Query_Decode(&qry, String_Slice(s, pos + 1), true);
		}
	}
}

def(void, ParseHeaderLine, String s) {
	if (hasCallback(this->events.onHeader)) {
		ssize_t pos;

		if ((pos = String_Find(s, $(": "))) != String_NotFound) {
			callback(this->events.onHeader,
				String_Trim(String_Slice(s, 0, pos)),   /* name  */
				String_Trim(String_Slice(s, pos + 2))); /* value */
		}
	}
}

/* Taken from Mongoose
 *
 * Portions Copyright (c) 2009 Gilbert Wellisch
 * Copyright (c) 2004-2009 Sergey Lyubka
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * Check whether full request is buffered. Return:
 *   -1         if request is malformed
 *    0         if request is not yet fully buffered
 *   >0         actual request length, including last \r\n\r\n
 */

sdef(ssize_t, GetLength, String str) {
	const char *s, *e;
	ssize_t len = 0;

	for (s = str.buf, e = s + str.len - 1; len <= 0 && s < e; s++) {
		/* Control characters are not allowed but characters >= 128 are. */
		if (!Char_IsPrintable(* (unsigned char *) s) && *s != '\r'
			&& *s != '\n' && * (unsigned char *) s < 128) {
			len = -1;
		} else if (s[0] == '\n' && s[1] == '\n') {
			len = (ssize_t) (s - str.buf) + 2;
		} else if (s[0] == '\n' && &s[1] < e && s[1] == '\r' && s[2] == '\n') {
			len = (ssize_t) (s - str.buf) + 3;
		}
	}

	return len;
}

def(void, Parse, ref(Type) type, String s) {
	ssize_t pos1stLine = String_Find(s, '\n');

	if (pos1stLine == String_NotFound) {
		throw(RequestMalformed);
	}

	ssize_t pos2ndLine = pos1stLine;

	if (s.buf[pos1stLine - 1] == '\r') {
		pos1stLine--;
	}

	if (type == ref(Type_Request)) {
		ssize_t posMethod = String_Find(s, ' ');

		if (posMethod == String_NotFound) {
			throw(RequestMalformed);
		}

		String method = String_Slice(s, 0, posMethod);
		call(ParseMethod, method);

		String version = String_Slice(s,
			pos1stLine - $("HTTP/1.1").len,
			$("HTTP/1.1").len);

		call(ParseVersion, version);

		String path = String_Slice(s,
			method.len + 1,
			pos1stLine - version.len - (method.len + 1) - 1);

		call(ParseUri, path);
	} else {
		ssize_t posVersion = String_Find(s, ' ');

		if (posVersion == String_NotFound) {
			throw(RequestMalformed);
		}

		String version = String_Slice(s, 0, posVersion);
		call(ParseVersion, version);

		ssize_t posCode = String_Find(s, posVersion + 1, ' ');

		if (posCode == String_NotFound) {
			throw(RequestMalformed);
		}

		String code = String_Slice(s, posVersion + 1, posCode - posVersion - 1);
		call(ParseStatus, code);
	}

	String res = HeapString(0);

	size_t len;
	size_t last = pos2ndLine;

	for (size_t i = pos2ndLine + 1; i < s.len; i++) {
		if (s.buf[i] == '\n') {
			if (s.buf[i - 1] == '\r') {
				len = i - last - 2;
			} else {
				len = i - last - 1;
			}

			if (len > 0) {
				String_Copy(&res,
					String_Slice(s, last + 1, len));

				try {
					call(ParseHeaderLine, res);
				} clean finally {
					if (e != 0) {
						String_Destroy(&res);
					}
				} tryEnd;
			}

			last = i;
		}
	}

	String_Destroy(&res);
}
