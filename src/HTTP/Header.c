#include "Header.h"

static ExceptionManager *exc;

Exception_Define(HTTP_Header_EmptyRequestUriException);
Exception_Define(HTTP_Header_RequestMalformedException);
Exception_Define(HTTP_Header_UnknownMethodException);
Exception_Define(HTTP_Header_UnknownStatusException);
Exception_Define(HTTP_Header_UnknownVersionException);

void HTTP_Header0(ExceptionManager *e) {
	exc = e;
}

void HTTP_Header_Init(HTTP_Header *this, HTTP_Header_Events events) {
	this->events = events;
}

void HTTP_Header_ParseMethod(HTTP_Header *this, String s) {
	if (s.len == 0) {
		throw(exc, &HTTP_Header_RequestMalformedException);
	}

	HTTP_Method method = HTTP_Method_FromString(s);

	if (method == HTTP_Method_Unset) {
		throw(exc, &HTTP_Header_UnknownMethodException);
	}

	if (this->events.onMethod != NULL) {
		this->events.onMethod(this->events.context, method);
	}
}

void HTTP_Header_ParseVersion(HTTP_Header *this, String s) {
	HTTP_Version version = HTTP_Version_FromString(s);

	if (version == HTTP_Version_Unset) {
		throw(exc, &HTTP_Header_UnknownVersionException);
	}

	if (this->events.onVersion != NULL) {
		this->events.onVersion(this->events.context, version);
	}
}

void HTTP_Header_ParseStatus(HTTP_Header *this, String s) {
	int code = Integer_ParseString(s);

	if (code == 0) {
		throw(exc, &HTTP_Header_UnknownStatusException);
	}

	HTTP_Status status = HTTP_Status_GetStatusByCode(code);

	if (status == HTTP_Status_Unset) {
		throw(exc, &HTTP_Header_UnknownStatusException);
	}

	if (this->events.onStatus != NULL) {
		this->events.onStatus(this->events.context, status);
	}
}

void HTTP_Header_ParseUri(HTTP_Header *this, String s) {
	if (s.len == 0) {
		throw(exc, &HTTP_Header_EmptyRequestUriException);
	}

	ssize_t pos = String_Find(&s, '?');

	if (pos == String_NotFound) {
		if (this->events.onPath != NULL) {
			this->events.onPath(this->events.context, s);
		}
	} else {
		if (this->events.onParameter != NULL) {
			return;
		}

		String params = String_Slice(&s, pos + 1);

		s.len = pos;
		this->events.onPath(this->events.context, s);

		try(exc) {
			HTTP_Query_Decode(
				params,
				false,
				this->events.onParameter,
				this->events.context);
		} finally {
			String_Destroy(&params);
		} tryEnd;
	}
}

void HTTP_Header_ParseHeaderLine(HTTP_Header *this, String s) {
	if (this->events.onHeader != NULL) {
		ssize_t pos;

		if ((pos = String_Find(&s, String(": "))) != String_NotFound) {
			String name  = String_Slice(&s, 0, pos);
			String value = String_Slice(&s, pos + 2);

			this->events.onHeader(this->events.context, name, value);

			String_Destroy(&value);
			String_Destroy(&name);
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

ssize_t HTTP_Header_GetLength(const char *buf, size_t buflen) {
	const char *s, *e;
	ssize_t len = 0;

	for (s = buf, e = s + buflen - 1; len <= 0 && s < e; s++) {
		/* Control characters are not allowed but characters >= 128 are. */
		if (!Char_IsPrintable(* (unsigned char *) s) && *s != '\r'
			&& *s != '\n' && * (unsigned char *) s < 128) {
			len = -1;
		} else if (s[0] == '\n' && s[1] == '\n') {
			len = (ssize_t) (s - buf) + 2;
		} else if (s[0] == '\n' && &s[1] < e && s[1] == '\r' && s[2] == '\n') {
			len = (ssize_t) (s - buf) + 3;
		}
	}

	return len;
}

void HTTP_Header_Parse(HTTP_Header *this, HTTP_Header_Type type, String s) {
	ssize_t pos1stLine = String_Find(&s, '\n');

	if (pos1stLine == String_NotFound) {
		throw(exc, &HTTP_Header_RequestMalformedException);
	}

	ssize_t pos2ndLine = pos1stLine;

	if (s.buf[pos1stLine - 1] == '\r') {
		pos1stLine--;
	}

	if (type == HTTP_Header_Type_Request) {
		ssize_t posMethod = String_Find(&s, ' ');

		if (posMethod == String_NotFound) {
			throw(exc, &HTTP_Header_RequestMalformedException);
		}

		String path    = HeapString(0);
		String method  = HeapString(0);
		String version = HeapString(0);

		try(exc) {
			method = String_Slice(&s, 0, posMethod);
			HTTP_Header_ParseMethod(this, method);

			version = String_Slice(&s, pos1stLine - sizeof("HTTP/1.1") + 1, sizeof("HTTP/1.1") - 1);
			HTTP_Header_ParseVersion(this, version);

			path = String_Slice(&s, method.len + 1, pos1stLine - version.len - (method.len + 1) - 1);
			HTTP_Header_ParseUri(this, path);
		} finally {
			String_Destroy(&version);
			String_Destroy(&method);
			String_Destroy(&path);
		} tryEnd;
	} else {
		ssize_t posVersion = String_Find(&s, ' ');

		if (posVersion == String_NotFound) {
			throw(exc, &HTTP_Header_RequestMalformedException);
		}

		String code    = HeapString(0);
		String version = HeapString(0);

		try(exc) {
			version = String_Slice(&s, 0, posVersion);
			HTTP_Header_ParseVersion(this, version);

			ssize_t posCode = String_Find(&s, posVersion + 1, ' ');

			if (posCode == String_NotFound) {
				throw(exc, &HTTP_Header_RequestMalformedException);
			}

			code = String_Slice(&s, posVersion + 1, posCode - posVersion - 1);
			HTTP_Header_ParseStatus(this, code);
		} finally {
			String_Destroy(&version);
			String_Destroy(&code);
		} tryEnd;
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
				String_Copy(&res, s, last + 1, len);

				try(exc) {
					HTTP_Header_ParseHeaderLine(this, res);
				} finally {
					String_Destroy(&res);
				} tryEnd;
			}

			last = i;
		}
	}
}
