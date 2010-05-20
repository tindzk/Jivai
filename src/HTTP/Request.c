#include "Request.h"

static ExceptionManager *exc;

Exception_Define(HTTP_Request_BodyTooLargeException);
Exception_Define(HTTP_Request_BodyUnexpectedException);
Exception_Define(HTTP_Request_EmptyRequestUriException);
Exception_Define(HTTP_Request_HeaderTooLargeException);
Exception_Define(HTTP_Request_RequestMalformedException);
Exception_Define(HTTP_Request_UnknownContentTypeException);
Exception_Define(HTTP_Request_UnknownMethodException);
Exception_Define(HTTP_Request_UnknownVersionException);

void HTTP_Request0(ExceptionManager *e) {
	exc = e;
}

void HTTP_Request_Init(HTTP_Request *this, size_t maxHeaderLength, uint64_t maxBodyLength) {
	this->headers.contentType = HTTP_Request_ContentType_Unset;
	this->headers.contentLength = 0;
	this->headers.persistentConnection = false;
	this->headers.boundary = HeapString(0);

	this->maxHeaderLength = maxHeaderLength;
	this->maxBodyLength   = maxBodyLength;

	this->state = HTTP_Request_State_Header;

	this->header = HeapString(maxHeaderLength);
	this->body   = HeapString(0);

	this->clean = false;
}

void HTTP_Request_Destroy(HTTP_Request *this) {
	String_Destroy(&this->header);
	String_Destroy(&this->body);
	String_Destroy(&this->headers.boundary);
}

void HTTP_Request_ParseHttpVersion(HTTP_Request *this, String version) {
	HTTP_Version httpver = HTTP_Version_1_1;

	if (String_Equals(&version, String("HTTP/1.1"))) {
		httpver = HTTP_Version_1_1;

		/* By default all connections in HTTP/1.1 are persistent. */
		this->headers.persistentConnection = true;
	} else if (String_Equals(&version, String("HTTP/1.0"))) {
		httpver = HTTP_Version_1_0;

		/* Persistent connections are not supported. */
		this->headers.persistentConnection = false;
	} else {
		throw(exc, &HTTP_Request_UnknownVersionException);
	}

	this->onHttpVersion(this->context, httpver);
}

void HTTP_Request_ParseMethod(HTTP_Request *this, String method) {
	if (method.len == 0) {
		throw(exc, &HTTP_Request_RequestMalformedException);
	}

	if (String_Equals(&method, String("GET"))) {
		this->method = HTTP_Method_Get;
	} else if (String_Equals(&method, String("POST"))) {
		this->method = HTTP_Method_Post;
	} else if (String_Equals(&method, String("HEAD"))) {
		this->method = HTTP_Method_Head;
	} else {
		throw(exc, &HTTP_Request_UnknownMethodException);
	}

	this->onMethod(this->context, this->method);
}

void HTTP_Request_ParseUri(HTTP_Request *this, String uri) {
	if (uri.len == 0) {
		throw(exc, &HTTP_Request_EmptyRequestUriException);
	}

	ssize_t pos;

	if ((pos = String_Find(&uri, '?')) == String_NotFound) {
		this->onPath(this->context, uri);
	} else {
		if (this->onQueryParameter == NULL) {
			return;
		}

		String params = String_Slice(&uri, pos + 1);

		uri.len = pos;
		this->onPath(this->context, uri);

		try(exc) {
			HTTP_Query_Decode(params, false, this->onQueryParameter, this->context);
		} finally {
			String_Destroy(&params);
		} tryEnd;
	}
}

void HTTP_Request_ParseHeaderLine(HTTP_Request *this, String s) {
	ssize_t pos;

	if ((pos = String_Find(&s, String(": "))) != String_NotFound) {
		String name  = String_Slice(&s, 0, pos);
		String value = String_Slice(&s, pos + 2);

		if (this->onHeader != NULL) {
			this->onHeader(this, name, value);
		}

		String_ToLower(&name);

		if (String_Equals(&name, String("connection"))) {
			String_ToLower(&value);

			StringArray chunks = String_SplitChar(&value, 0, ',');

			for (size_t i = 0; i < chunks.len; i++) {
				String_Trim(&chunks.buf[i]);

				if (String_Equals(&value, String("close"))) {
					this->headers.persistentConnection = false;
				} else if (String_Equals(&value, String("keep-alive"))) {
					this->headers.persistentConnection = true;
				}
			}

			StringArray_Destroy(&chunks);
		} else {
			if (this->method == HTTP_Method_Post) {
				String_ToLower(&value);

				if (String_Equals(&name, String("content-type"))) {
					if (String_BeginsWith(&value, String("application/x-www-form-urlencoded"))) {
						this->headers.contentType = HTTP_Request_ContentType_SinglePart;
					} else if (String_BeginsWith(&value, String("multipart/form-data"))) {
						this->headers.contentType = HTTP_Request_ContentType_MultiPart;

						ssize_t posBoundary = String_Find(
							&value,
							sizeof("multipart/form-data") - 1,
							String("boundary="));

						if (posBoundary == String_NotFound) {
							throw(exc, &HTTP_Request_UnknownContentTypeException);
						}

						String_Copy(&this->headers.boundary, value, posBoundary + sizeof("boundary=") - 1);
					} else {
						throw(exc, &HTTP_Request_UnknownContentTypeException);
					}
				} else if (String_Equals(&name, String("content-length"))) {
					if (this->method != HTTP_Method_Post) {
						throw(exc, &HTTP_Request_BodyUnexpectedException);
					}

					this->headers.contentLength = Integer64_ParseString(value);

					if (this->headers.contentLength > this->maxBodyLength) {
						throw(exc, &HTTP_Request_BodyTooLargeException);
					}
				}
			}
		}

		String_Destroy(&name);
		String_Destroy(&value);
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

ssize_t HTTP_Request_GetLength(const char *buf, size_t buflen) {
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

HTTP_Request_Result HTTP_Request_ReadHeader(HTTP_Request *this, SocketConnection *conn) {
	if (this->clean) {
		/* Clean up variables from previous requests. */
		this->headers.contentLength = 0;
		this->headers.contentType = HTTP_Request_ContentType_Unset;

		this->clean = false;
	}

	if (this->body.buf != NULL) {
		String_Destroy(&this->body);
	}

	if (this->headers.boundary.buf != NULL) {
		String_Destroy(&this->headers.boundary);
	}

	ssize_t requestOffset = 0;

	while (this->header.len < this->header.size) {
		/* Do this now because the buffer might already contain the next
		 * request. */
		requestOffset = HTTP_Request_GetLength(this->header.buf, this->header.len);

		if (requestOffset == -1) {
			/* The request is malformed. */
			this->header.len = 0;
			return HTTP_Request_Result_Error;
		} else if (requestOffset > 0) {
			/* The request is complete. */
			break;
		}

		if (this->header.size - this->header.len == 0) {
			/* The buffer is full, but the request is still incomplete. */
			throw(exc, &HTTP_Request_HeaderTooLargeException);
		}

		size_t len = 0;
		bool incomplete = false;

		try (exc) {
			len = SocketConnection_Read(conn,
				this->header.buf  + this->header.len,
				this->header.size - this->header.len
			);
		} catch(&SocketConnection_EmptyQueueException, e) {
			incomplete = true;
		} finally {

		} tryEnd;

		if (incomplete) {
			/* This function will be called again when more data is available. */
			return HTTP_Request_Result_Incomplete;
		}

		this->header.len += len;
	}

	/* This is the case when this->header.len >= this->header.size. */
	if (requestOffset == 0) {
		requestOffset = HTTP_Request_GetLength(this->header.buf, this->header.len);

		if (requestOffset == -1) {
			/* The request is malformed. */
			this->header.len = 0;
			return HTTP_Request_Result_Error;
		} else if (requestOffset == 0) {
			/* The buffer is full, but the request is still incomplete. */
			throw(exc, &HTTP_Request_HeaderTooLargeException);
		}
	}

	/* Trim the request... */
	size_t oldLength = this->header.len;
	String_TrimLeft(&this->header);
	requestOffset -= oldLength - this->header.len; /* ...and update requestOffset accordingly. */

	String s = StackString(0);
	s.buf = this->header.buf;
	s.len = requestOffset;

	ssize_t pos1stLine = 0;
	if ((pos1stLine = String_Find(&s, String("\n"))) == String_NotFound) {
		throw(exc, &HTTP_Request_RequestMalformedException);
	}

	ssize_t pos2ndLine = pos1stLine;
	if (s.buf[pos1stLine - 1] == '\r') {
		pos1stLine--;
	}

	ssize_t posMethod;
	if ((posMethod = String_Find(&s, 0, 5, String(" "))) == String_NotFound) {
		throw(exc, &HTTP_Request_RequestMalformedException);
	}

	String method  = HeapString(0);
	String path    = HeapString(0);
	String httpver = HeapString(0);

	try(exc) {
		method = String_Slice(&s, 0, posMethod);
		HTTP_Request_ParseMethod(this, method);

		httpver = String_Slice(&s, pos1stLine - sizeof("HTTP/1.1") + 1, sizeof("HTTP/1.1") - 1);
		HTTP_Request_ParseHttpVersion(this, httpver);

		path = String_Slice(&s, method.len + 1, pos1stLine - httpver.len - (method.len + 1) - 1);
		HTTP_Request_ParseUri(this, path);
	} finally {
		String_Destroy(&method);
		String_Destroy(&httpver);
		String_Destroy(&path);
	} tryEnd;

	String res = HeapString(0);

	size_t len, last;
	for (size_t i = last = pos2ndLine; i < s.len; i++) {
		if (s.buf[i] == '\n') {
			if (s.buf[i - 1] == '\r') {
				len = i - last - 2;
			} else {
				len = i - last - 1;
			}

			if (len > 0) {
				String_Copy(&res, s, last + 1, len);

				try(exc) {
					HTTP_Request_ParseHeaderLine(this, res);
				} finally {
					String_Destroy(&res);
				} tryEnd;
			}

			last = i;
		}
	}

	if (this->headers.contentLength > 0) {
		/* The request has a body. */
		this->body = HeapString(this->headers.contentLength);

		/* It is likely that we already have fetched some body chunks in our header buffer. */
		if (this->header.len != (size_t) requestOffset) {
			if (this->header.len - requestOffset >= this->headers.contentLength) {
				/* We have the whole body in this->header. */
				String_Copy(&this->body, this->header, requestOffset, this->headers.contentLength);

				/* In this->header there is more data which does not belong to the body.
				 * Probably it's already the next request.
				 */
				String_Crop(&this->header, requestOffset + this->headers.contentLength);
			} else {
				/* The body is only partial. */
				String_Copy(&this->body, this->header, requestOffset);

				/* See comment below. */
				this->header.len = 0;
			}
		} else {
			/* The header buffer only contains the current
			 * header which has been just processed. Clear the
			 * buffer, otherwise this function will be called again
			 * without requesting more data from the peer. However,
			 * as this->headers.contentLength > 0 is still valid
			 * then, the next request will be handled in
			 * HTTP_Request_ReadBody() and is assumed to be a body
			 * instead of headers.
			 */

			this->header.len = 0;
		}

		this->state = HTTP_Request_State_Body;
	} else {
		/* The buffer possibly contains the next request. This will
		 * delete the range 0..requestOffset.
		 *
		 * If this->header does not hold anything except for the
		 * current requestion, String_Crop() empties the string.
		 */

		String_Crop(&this->header, requestOffset);

		this->state = HTTP_Request_State_Dispatch;
	}

	return HTTP_Request_Result_Complete;
}

HTTP_Request_Result HTTP_Request_ReadBody(HTTP_Request *this, SocketConnection *conn) {
	/* The next time HTTP_Request_ReadHeader() gets called, some headers
	 * referring to the current request will be cleared.
	 */
	this->clean = true;

	if (this->headers.contentType == HTTP_Request_ContentType_MultiPart) {
		/* TODO */
	} else {
		/* Get the remaining chunks (if any). */
		while (this->body.len < this->body.size) {
			size_t len = 0;
			bool incomplete = false;

			try (exc) {
				len = SocketConnection_Read(conn,
					this->body.buf  + this->body.len,
					this->body.size - this->body.len
				);
			} catch(&SocketConnection_EmptyQueueException, e) {
				incomplete = true;
			} finally {

			} tryEnd;

			if (incomplete) {
				return HTTP_Request_Result_Incomplete;
			}

			this->body.len += len;
		}

		try(exc) {
			/* Handle the form data. */
			if (this->onBodyParameter != NULL) {
				HTTP_Query_Decode(this->body, true, this->onBodyParameter, this->context);
			}
		} finally {
			this->state = HTTP_Request_State_Header;
		} tryEnd;
	}

	this->state = HTTP_Request_State_Dispatch;

	return HTTP_Request_Result_Complete;
}

HTTP_Request_Result HTTP_Request_Dispatch(HTTP_Request *this) {
	bool keepOpen = this->onRespond(this->context, this->headers.persistentConnection);

	this->state = HTTP_Request_State_Header;

	if (keepOpen) {
		return HTTP_Request_Result_Complete;
	}

	return HTTP_Request_Result_Error;
}

bool HTTP_Request_Parse(HTTP_Request *this, SocketConnection *conn) {
	HTTP_Request_Result res = HTTP_Request_Result_Error;

	if (this->state == HTTP_Request_State_Header) {
		res = HTTP_Request_ReadHeader(this, conn);
	} else if (this->state == HTTP_Request_State_Body) {
		res = HTTP_Request_ReadBody(this, conn);
	} else if (this->state == HTTP_Request_State_Dispatch) {
		res = HTTP_Request_Dispatch(this);
	}

	if (res == HTTP_Request_Result_Complete) {
		/* Try to handle the next state now. */
		return HTTP_Request_Parse(this, conn);
	}

	/* Keep the connection open if more data is required. */
	return res == HTTP_Request_Result_Incomplete;
}
