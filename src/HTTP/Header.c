#import "Header.h"

#define self HTTP_Header

rsdef(self, New, ref(Events) events) {
	return (self) {
		.events = events,
		.pos1stLine = -1,
		.pos2ndLine = -1
	};
}

static def(void, PreParse, RdString s) {
	this->pos1stLine = String_Find(s, '\n');
	this->pos2ndLine = this->pos1stLine;

	if (this->pos1stLine == String_NotFound) {
		throw(RequestMalformed);
	}

	if (s.buf[this->pos1stLine - 1] == '\r') {
		this->pos1stLine--;
	}
}

def(HTTP_Method, ParseMethod, RdString s) {
	if (s.len == 0) {
		throw(RequestMalformed);
	}

	HTTP_Method method = HTTP_Method_FromString(s);

	if (method == HTTP_Method_Unset) {
		throw(UnknownMethod);
	}

	return method;
}

def(HTTP_Version, ParseVersion, RdString s) {
	HTTP_Version version = HTTP_Version_FromString(s);

	if (version == HTTP_Version_Unset) {
		throw(UnknownVersion);
	}

	return version;
}

def(HTTP_Status, ParseStatus, RdString s) {
	s32 code = 0;

	try {
		code = Int32_Parse(s);
	} catchModule(Integer) {

	} finally {

	} tryEnd;

	if (code == 0) {
		throw(UnknownStatus);
	}

	HTTP_Status status = HTTP_Status_GetStatusByCode(code);

	if (status == HTTP_Status_Unset) {
		throw(UnknownStatus);
	}

	return status;
}

def(CarrierString, ParseUri, RdString s) {
	if (s.len == 0) {
		throw(EmptyRequestUri);
	}

	ssize_t pos = String_Find(s, '?');

	RdString path =
		(pos != String_NotFound)
			? String_Slice(s, 0, pos)
			: s;

	size_t len = HTTP_Query_GetAbsoluteLength(path);

	if (len <= path.len) {
		HTTP_Query_Unescape(path, path.buf, true);
		path.len = len;

		return String_ToCarrier(RdString_Exalt(path));
	} else {
		String decoded = String_New(len);
		HTTP_Query_Unescape(path, decoded.buf, true);
		decoded.len = len;

		return String_ToCarrier(decoded);
	}
}

def(void, ParseUriParameters, RdString s) {
	ssize_t pos = String_Find(s, '?');

	if (hasCallback(this->events.onParameter)) {
		if (pos != String_NotFound) {
			HTTP_Query qry;
			HTTP_Query_Init(&qry, this->events.onParameter);
			HTTP_Query_SetAutoResize(&qry, true);
			HTTP_Query_Decode(&qry, String_Slice(s, pos + 1), true);
		}
	}
}

def(void, ParseHeaderLine, RdString s) {
	if (hasCallback(this->events.onHeader)) {
		RdString name, value;
		if (String_Parse($("%: %"), s, &name, &value)) {
			callback(this->events.onHeader,
				String_Trim(name),
				String_Trim(value));
		}
	}
}

static def(void, ParseHeaders, RdString s) {
	size_t len;
	size_t last = this->pos2ndLine;

	for (size_t i = this->pos2ndLine + 1; i < s.len; i++) {
		if (s.buf[i] == '\n') {
			if (s.buf[i - 1] == '\r') {
				len = i - last - 2;
			} else {
				len = i - last - 1;
			}

			if (len > 0) {
				call(ParseHeaderLine, String_Slice(s, last + 1, len));
			}

			last = i;
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

sdef(ssize_t, GetLength, RdString str) {
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

def(void, ParseRequest, RdString s) {
	call(PreParse, s);

	RdString line = String_Slice(s, 0, this->pos1stLine);

	RdString method, uri, version;
	if (!String_Parse($("% % %"), line, &method, &uri, &version)) {
		throw(RequestMalformed);
	}

	CarrierString path = call(ParseUri, uri);

	try {
		HTTP_RequestInfo info = {
			.version = call(ParseVersion, version),
			.method  = call(ParseMethod, method),
			.path    = path.rd
		};

		callback(this->events.onRequestInfo, info);
	} finally {
		CarrierString_Destroy(&path);
	} tryEnd;

	call(ParseUriParameters, uri);
	call(ParseHeaders, s);
}

def(void, ParseResponse, RdString s) {
	call(PreParse, s);

	RdString version, code;

	if (!String_Parse($("% % "), s, &version, &code)) {
		throw(ResponseMalformed);
	}

	callback(this->events.onResponseInfo, (HTTP_ResponseInfo) {
		.version = call(ParseVersion, version),
		.status  = call(ParseStatus,  code)
	});

	call(ParseHeaders, s);
}
