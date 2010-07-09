#include "Server.h"

static ExceptionManager *exc;

Exception_Define(HTTP_Server_BodyTooLargeException);
Exception_Define(HTTP_Server_BodyUnexpectedException);
Exception_Define(HTTP_Server_HeaderTooLargeException);
Exception_Define(HTTP_Server_UnknownContentTypeException);

void HTTP_Server0(ExceptionManager *e) {
	exc = e;
}

void HTTP_Server_Init(HTTP_Server *this, HTTP_Server_Events events, SocketConnection *conn, size_t maxHeaderLength, uint64_t maxBodyLength) {
	this->events = events;

	this->headers.boundary = HeapString(0);

	this->headers.contentType   = HTTP_ContentType_Unset;
	this->headers.contentLength = 0;

	this->maxBodyLength   = maxBodyLength;
	this->maxHeaderLength = maxHeaderLength;

	this->body   = HeapString(0);
	this->header = HeapString(maxHeaderLength);

	this->state  = HTTP_Server_State_Header;
	this->conn   = conn;
	this->clean  = false;

	this->headers.persistentConnection = false;
}

void HTTP_Server_Destroy(HTTP_Server *this) {
	String_Destroy(&this->header);
	String_Destroy(&this->body);
	String_Destroy(&this->headers.boundary);
}

void HTTP_Server_OnMethod(HTTP_Server *this, HTTP_Method method) {
	this->method = method;

	if (this->events.onMethod != NULL) {
		this->events.onMethod(this->events.context, method);
	}
}

void HTTP_Server_OnVersion(HTTP_Server *this, HTTP_Version version) {
	if (version == HTTP_Version_1_1) {
		/* By default all connections in HTTP/1.1 are persistent. */
		this->headers.persistentConnection = true;
	} else if (version == HTTP_Version_1_0) {
		/* Persistent connections are not supported. */
		this->headers.persistentConnection = false;
	}

	if (this->events.onVersion != NULL) {
		this->events.onVersion(this->events.context, version);
	}
}

void HTTP_Server_OnPath(HTTP_Server *this, String path) {
	if (this->events.onPath != NULL) {
		this->events.onPath(this->events.context, path);
	}
}

String* HTTP_Server_OnQueryParameter(HTTP_Server *this, String name) {
	if (this->events.onQueryParameter != NULL) {
		return this->events.onQueryParameter(this->events.context, name);
	}

	return NULL;
}

void HTTP_Server_OnHeader(HTTP_Server *this, String name, String value) {
	if (this->events.onHeader != NULL) {
		this->events.onHeader(this->events.context, name, value);
	}

	String_ToLower(&name);

	if (String_Equals(name, String("connection"))) {
		String_ToLower(&value);

		StringArray chunks = String_Split(value, 0, ',');

		for (size_t i = 0; i < chunks.len; i++) {
			String_Trim(&chunks.buf[i]);

			if (String_Equals(chunks.buf[i], String("close"))) {
				this->headers.persistentConnection = false;
			} else if (String_Equals(chunks.buf[i], String("keep-alive"))) {
				this->headers.persistentConnection = true;
			}
		}

		StringArray_Destroy(&chunks);
	} else {
		if (this->method == HTTP_Method_Post) {
			String_ToLower(&value);

			if (String_Equals(name, String("content-type"))) {
				if (String_BeginsWith(value, String("application/x-www-form-urlencoded"))) {
					this->headers.contentType = HTTP_ContentType_SinglePart;
				} else if (String_BeginsWith(value, String("multipart/form-data"))) {
					this->headers.contentType = HTTP_ContentType_MultiPart;

					ssize_t posBoundary = String_Find(value,
						String("multipart/form-data").len,
						String("boundary="));

					if (posBoundary == String_NotFound) {
						throw(exc, &HTTP_Server_UnknownContentTypeException);
					}

					String_Copy(&this->headers.boundary, value, posBoundary + sizeof("boundary=") - 1);
				} else {
					throw(exc, &HTTP_Server_UnknownContentTypeException);
				}
			} else if (String_Equals(name, String("content-length"))) {
				if (this->method != HTTP_Method_Post) {
					throw(exc, &HTTP_Server_BodyUnexpectedException);
				}

				this->headers.contentLength = Integer64_ParseString(value);

				if (this->headers.contentLength > this->maxBodyLength) {
					throw(exc, &HTTP_Server_BodyTooLargeException);
				}
			}
		}
	}
}

HTTP_Server_Result HTTP_Server_ReadHeader(HTTP_Server *this) {
	if (this->clean) {
		/* Clean up variables from previous requests. */
		this->headers.contentLength = 0;
		this->headers.contentType = HTTP_ContentType_Unset;

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
		requestOffset = HTTP_Header_GetLength(this->header);

		if (requestOffset == -1) {
			/* The request is malformed. */
			this->header.len = 0;
			return HTTP_Server_Result_Error;
		} else if (requestOffset > 0) {
			/* The request is complete. */
			break;
		}

		if (this->header.size - this->header.len == 0) {
			/* The buffer is full, but the request is still incomplete. */
			throw(exc, &HTTP_Server_HeaderTooLargeException);
		}

		size_t len = 0;
		bool incomplete = false;

		try (exc) {
			len = SocketConnection_Read(this->conn,
				this->header.buf  + this->header.len,
				this->header.size - this->header.len
			);
		} catch(&SocketConnection_EmptyQueueException, e) {
			incomplete = true;
		} finally {

		} tryEnd;

		if (incomplete) {
			/* This function will be called again when more data is available. */
			return HTTP_Server_Result_Incomplete;
		}

		this->header.len += len;
	}

	/* This is the case when this->header.len >= this->header.size. */
	if (requestOffset == 0) {
		requestOffset = HTTP_Header_GetLength(this->header);

		if (requestOffset == -1) {
			/* The request is malformed. */
			this->header.len = 0;
			return HTTP_Server_Result_Error;
		} else if (requestOffset == 0) {
			/* The buffer is full, but the request is still incomplete. */
			throw(exc, &HTTP_Server_HeaderTooLargeException);
		}
	}

	/* Trim the request... */
	size_t oldLength = this->header.len;
	String_TrimLeft(&this->header);
	requestOffset -= oldLength - this->header.len; /* ...and update requestOffset accordingly. */

	HTTP_Header_Events events;
	events.onMethod    = (void *) &HTTP_Server_OnMethod;
	events.onVersion   = (void *) &HTTP_Server_OnVersion;
	events.onPath      = (void *) &HTTP_Server_OnPath;
	events.onParameter = (void *) &HTTP_Server_OnQueryParameter;
	events.onHeader    = (void *) &HTTP_Server_OnHeader;
	events.context     = this;

	String s = String_FastSlice(this->header, 0, requestOffset);

	HTTP_Header header;
	HTTP_Header_Init(&header, events);
	HTTP_Header_Parse(&header, HTTP_Header_Type_Request, s);

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
			 * HTTP_Server_ReadBody() and is assumed to be a body
			 * instead of headers.
			 */

			this->header.len = 0;
		}

		this->state = HTTP_Server_State_Body;
	} else {
		/* The buffer possibly contains the next request. This will
		 * delete the range 0..requestOffset.
		 *
		 * If this->header does not hold anything except for the
		 * current requestion, String_Crop() empties the string.
		 */

		String_Crop(&this->header, requestOffset);

		this->state = HTTP_Server_State_Dispatch;
	}

	return HTTP_Server_Result_Complete;
}

HTTP_Server_Result HTTP_Server_ReadBody(HTTP_Server *this) {
	/* The next time HTTP_Server_ReadHeader() gets called, some headers
	 * referring to the current request will be cleared.
	 */
	this->clean = true;

	if (this->headers.contentType == HTTP_ContentType_MultiPart) {
		/* TODO */
	} else {
		/* Get the remaining chunks (if any). */
		while (this->body.len < this->body.size) {
			size_t len = 0;
			bool incomplete = false;

			try (exc) {
				len = SocketConnection_Read(this->conn,
					this->body.buf  + this->body.len,
					this->body.size - this->body.len);
			} catch(&SocketConnection_EmptyQueueException, e) {
				incomplete = true;
			} finally {

			} tryEnd;

			if (incomplete) {
				return HTTP_Server_Result_Incomplete;
			}

			this->body.len += len;
		}

		try(exc) {
			/* Handle the form data. */
			if (this->events.onBodyParameter != NULL) {
				HTTP_Query qry;
				HTTP_Query_Init(&qry, this->events.onBodyParameter, this->events.context);
				HTTP_Query_Decode(&qry, this->body, true);
			}
		} finally {
			this->state = HTTP_Server_State_Header;
		} tryEnd;
	}

	this->state = HTTP_Server_State_Dispatch;

	return HTTP_Server_Result_Complete;
}

bool HTTP_Server_Process(HTTP_Server *this) {
	HTTP_Server_Result res = HTTP_Server_Result_Error;

	if (this->state == HTTP_Server_State_Header) {
		res = HTTP_Server_ReadHeader(this);
	} else if (this->state == HTTP_Server_State_Body) {
		res = HTTP_Server_ReadBody(this);
	} else if (this->state == HTTP_Server_State_Dispatch) {
		this->events.onRespond(this->events.context, this->headers.persistentConnection);
		this->state = HTTP_Server_State_Header;
		return false;
	}

	if (res == HTTP_Server_Result_Complete) {
		/* Try to handle the next state now. */
		return HTTP_Server_Process(this);
	}

	/* Keep the connection open if more data is required. */
	return res == HTTP_Server_Result_Incomplete;
}
