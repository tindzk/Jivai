#import "Server.h"

#define self HTTP_Server

def(void, Init, SocketConnection *conn, size_t maxHeaderLength, u64 maxBodyLength) {
	this->headers.boundary = HeapString(0);

	this->headers.contentType   = HTTP_ContentType_Unset;
	this->headers.contentLength = 0;

	this->maxBodyLength   = maxBodyLength;
	this->maxHeaderLength = maxHeaderLength;

	this->body   = HeapString(0);
	this->header = HeapString(maxHeaderLength);

	this->state   = ref(State_Header);
	this->conn    = conn;
	this->cleanup = false;

	this->headers.persistentConnection = false;

	this->events = (typeof(this->events)) {
		.onMethod = { EmptyCallback() }
	};
}

def(void, Destroy) {
	String_Destroy(&this->header);
	String_Destroy(&this->body);
	String_Destroy(&this->headers.boundary);
}

def(void, BindMethod, HTTP_OnMethod onMethod) {
	this->events.onMethod = onMethod;
}

def(void, BindVersion, HTTP_OnVersion onVersion) {
	this->events.onVersion = onVersion;
}

def(void, BindPath, HTTP_OnPath onPath) {
	this->events.onPath = onPath;
}

def(void, BindHeader, HTTP_OnHeader onHeader) {
	this->events.onHeader = onHeader;
}

def(void, BindBodyParameter, HTTP_OnParameter onBodyParameter) {
	this->events.onBodyParameter = onBodyParameter;
}

def(void, BindQueryParameter, HTTP_OnParameter onQueryParameter) {
	this->events.onQueryParameter = onQueryParameter;
}

def(void, BindRespond, ref(OnRespond) onRespond) {
	this->events.onRespond = onRespond;
}

static def(void, OnMethod, HTTP_Method method) {
	this->method = method;
	callback(this->events.onMethod, method);
}

static def(void, OnVersion, HTTP_Version version) {
	if (version == HTTP_Version_1_1) {
		/* By default all connections in HTTP/1.1 are persistent. */
		this->headers.persistentConnection = true;
	} else if (version == HTTP_Version_1_0) {
		/* Persistent connections are not supported. */
		this->headers.persistentConnection = false;
	}

	callback(this->events.onVersion, version);
}

static def(void, OnHeader, String name, String value) {
	callback(this->events.onHeader, name, value);

	/* Generally, manipulating the `mutable' property is not
	 * advisable but in this case both strings are known to be
	 * heap-allocated and also keep their lengths.
	 */
	name.mutable  = true;
	value.mutable = true;

	String_ToLower(&name);

	if (String_Equals(name, String("connection"))) {
		String_ToLower(&value);

		StringArray *chunks = String_Split(value, ',');

		for (size_t i = 0; i < chunks->len; i++) {
			String tmp = String_Trim(chunks->buf[i]);

			if (String_Equals(tmp, String("close"))) {
				this->headers.persistentConnection = false;
			} else if (String_Equals(tmp, String("keep-alive"))) {
				this->headers.persistentConnection = true;
			}
		}

		Array_Destroy(chunks);
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
						throw(excUnknownContentType);
					}

					String_Copy(&this->headers.boundary, value, posBoundary + sizeof("boundary=") - 1);
				} else {
					throw(excUnknownContentType);
				}
			} else if (String_Equals(name, String("content-length"))) {
				if (this->method != HTTP_Method_Post) {
					throw(excBodyUnexpected);
				}

				this->headers.contentLength = Int64_Parse(value);

				if (this->headers.contentLength > this->maxBodyLength) {
					throw(excBodyTooLarge);
				}
			}
		}
	}
}

def(ref(Result), ReadHeader) {
	if (this->cleanup) {
		/* Clean up variables from previous requests. */
		this->headers.contentLength = 0;
		this->headers.contentType = HTTP_ContentType_Unset;

		this->cleanup = false;
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
			return ref(Result_Error);
		} else if (requestOffset > 0) {
			/* The request is complete. */
			break;
		}

		if (this->header.size - this->header.len == 0) {
			/* The buffer is full, but the request is still incomplete. */
			throw(excHeaderTooLarge);
		}

		ssize_t len = SocketConnection_Read(this->conn,
			this->header.buf  + this->header.len,
			this->header.size - this->header.len);

		if (len <= 0) {
			/* This function will be called again when more data is available. */
			return ref(Result_Incomplete);
		}

		this->header.len += len;
	}

	/* This is the case when this->header.len >= this->header.size. */
	if (requestOffset == 0) {
		requestOffset = HTTP_Header_GetLength(this->header);

		if (requestOffset == -1) {
			/* The request is malformed. */
			this->header.len = 0;
			return ref(Result_Error);
		} else if (requestOffset == 0) {
			/* The buffer is full, but the request is still incomplete. */
			throw(excHeaderTooLarge);
		}
	}

	/* Trim the request... */
	size_t oldLength = this->header.len;
	String_Trim(&this->header, String_TrimLeft);
	requestOffset -= oldLength - this->header.len; /* ...and update requestOffset accordingly. */

	HTTP_Header_Events events;
	events.onMethod    = (HTTP_OnMethod) Callback(this, ref(OnMethod));
	events.onVersion   = (HTTP_OnVersion) Callback(this, ref(OnVersion));
	events.onPath      = this->events.onPath;
	events.onParameter = this->events.onQueryParameter;
	events.onHeader    = (HTTP_OnHeader) Callback(this, ref(OnHeader));

	String s = String_Slice(this->header, 0, requestOffset);

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
			 * ReadBody() and is assumed to be a body instead of
			 * headers.
			 */

			this->header.len = 0;
		}

		this->state = ref(State_Body);
	} else {
		/* The buffer possibly contains the next request. This will
		 * delete the range 0..requestOffset.
		 *
		 * If this->header does not hold anything except for the
		 * current requestion, String_Crop() empties the string.
		 */

		String_Crop(&this->header, requestOffset);

		this->state = ref(State_Dispatch);
	}

	return ref(Result_Complete);
}

def(ref(Result), ReadBody) {
	/* The next time ReadHeader() gets called, some headers
	 * referring to the current request will be cleared.
	 */
	this->cleanup = true;

	if (this->headers.contentType == HTTP_ContentType_MultiPart) {
		/* TODO */
	} else {
		/* Get the remaining chunks (if any). */
		while (this->body.len < this->body.size) {
			ssize_t len = SocketConnection_Read(this->conn,
				this->body.buf  + this->body.len,
				this->body.size - this->body.len);

			if (len == -1) {
				return ref(Result_Incomplete);
			} else if (len == 0) {
				break;
			}

			this->body.len += len;
		}

		try {
			/* Handle the form data. */
			if (hasCallback(this->events.onBodyParameter)) {
				HTTP_Query qry;
				HTTP_Query_Init(&qry, this->events.onBodyParameter);
				HTTP_Query_SetAutoResize(&qry, true);
				HTTP_Query_Decode(&qry, this->body, true);
			}
		} clean finally {
			this->state = ref(State_Header);
		} tryEnd;
	}

	this->state = ref(State_Dispatch);

	return ref(Result_Complete);
}

def(bool, Process) {
	ref(Result) res = ref(Result_Error);

	if (this->state == ref(State_Header)) {
		res = call(ReadHeader);
	} else if (this->state == ref(State_Body)) {
		res = call(ReadBody);
	} else if (this->state == ref(State_Dispatch)) {
		callback(this->events.onRespond, this->headers.persistentConnection);
		this->state = ref(State_Header);
		return false;
	}

	if (res == ref(Result_Complete)) {
		/* Try to handle the next state now. */
		return call(Process);
	}

	/* Keep the connection open if more data is required. */
	return res == ref(Result_Incomplete);
}
