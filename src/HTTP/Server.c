#import "Server.h"

#define self HTTP_Server

rsdef(self, New, SocketConnection *conn, size_t maxHeaderLength, u64 maxBodyLength) {
	return (self) {
		.headers.boundary = String_New(0),

		.headers.contentType   = HTTP_ContentType_Unset,
		.headers.contentLength = 0,

		.maxBodyLength   = maxBodyLength,
		.maxHeaderLength = maxHeaderLength,

		.body   = String_New(0),
		.header = String_New(maxHeaderLength),

		.state      = ref(State_Header),
		.conn       = conn,
		.newRequest = true,

		.headers.persistentConnection = false,

		.events = {
			.onMethod = { .cb = NULL }
		}
	};
}

def(void, Destroy) {
	String_Destroy(&this->header);
	String_Destroy(&this->body);
	String_Destroy(&this->headers.boundary);
}

def(void, BindRequest, ref(OnRequest) onRequest) {
	this->events.onRequest = onRequest;
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

static def(void, OnHeader, RdString name, RdString value) {
	callback(this->events.onHeader, name, value);

	String_ToLower((String *) &name);

	if (String_Equals(name, $("connection"))) {
		String_ToLower((String *) &value);

		RdString elem = $("");
		while (String_Split(value, ',', &elem)) {
			RdString chunk = String_Trim(elem);

			if (String_Equals(chunk, $("close"))) {
				this->headers.persistentConnection = false;
			} else if (String_Equals(chunk, $("keep-alive"))) {
				this->headers.persistentConnection = true;
			}
		}
	} else {
		if (this->method == HTTP_Method_Post) {
			String_ToLower((String *) &value);

			if (String_Equals(name, $("content-type"))) {
				if (String_BeginsWith(value, $("application/x-www-form-urlencoded"))) {
					this->headers.contentType = HTTP_ContentType_SinglePart;
				} else if (String_BeginsWith(value, $("multipart/form-data"))) {
					this->headers.contentType = HTTP_ContentType_MultiPart;

					ssize_t posBoundary = String_Find(value,
						$("multipart/form-data").len,
						$("boundary="));

					if (posBoundary == String_NotFound) {
						throw(UnknownContentType);
					}

					String_Copy(&this->headers.boundary,
						String_Slice(value, posBoundary + $("boundary=").len));
				} else {
					throw(UnknownContentType);
				}
			} else if (String_Equals(name, $("content-length"))) {
				if (this->method != HTTP_Method_Post) {
					throw(BodyUnexpected);
				}

				try {
					this->headers.contentLength = UInt64_Parse(value);
				} catchModule(Integer) {
					throw(BodyTooLarge);
				} finally {

				} tryEnd;

				if (this->headers.contentLength > this->maxBodyLength) {
					throw(BodyTooLarge);
				}
			}
		}
	}
}

def(ref(Result), ReadHeader) {
	if (this->newRequest) {
		callback(this->events.onRequest);

		/* Clean up variables from previous requests. */
		this->headers.contentLength = 0;
		this->headers.contentType = HTTP_ContentType_Unset;

		this->newRequest = false;
	}

	if (this->body.buf != NULL) {
		String_Destroy(&this->body);
		this->body = String_New(0);
	}

	if (this->headers.boundary.buf != NULL) {
		String_Destroy(&this->headers.boundary);
		this->headers.boundary = String_New(0);
	}

	ssize_t requestOffset = 0;

	for (;;) {
		/* Do this now because the buffer might already contain the next
		 * request. */
		requestOffset = HTTP_Header_GetLength(this->header.rd);

		if (requestOffset == -1) {
			/* The request is malformed. */
			this->header.len = 0;
			return ref(Result_Error);
		} else if (requestOffset > 0) {
			/* The request is complete. */
			break;
		}

		size_t free = String_GetFree(this->header);

		if (free == 0) {
			/* The buffer is full, but the request is still incomplete. */
			throw(HeaderTooLarge);
		}

		ssize_t len = SocketConnection_Read(this->conn,
			this->header.buf + this->header.len, free);

		if (len <= 0) {
			/* This function will be called again when more data is available. */
			return ref(Result_Incomplete);
		}

		this->header.len += len;
	}

	/* This is the case when this->header.len >= this->header.size. */
	if (requestOffset == 0) {
		requestOffset = HTTP_Header_GetLength(this->header.rd);

		if (requestOffset == -1) {
			/* The request is malformed. */
			this->header.len = 0;
			return ref(Result_Error);
		} else if (requestOffset == 0) {
			/* The buffer is full, but the request is still incomplete. */
			throw(HeaderTooLarge);
		}
	}

	/* The next time ReadHeader() gets called, the `onRequest' event will be
	 * invoked and headers referring to the current request will be cleared.
	 */
	this->newRequest = true;

	/* Trim the request and update requestOffset accordingly. */
	RdString cleaned = String_Trim(this->header.rd, String_TrimLeft);
	size_t ofs = this->header.len - cleaned.len;
	String_Crop(&this->header, ofs);
	requestOffset -= ofs;

	HTTP_Header_Events events;
	events.onMethod    = HTTP_OnMethod_For(this, ref(OnMethod));
	events.onVersion   = HTTP_OnVersion_For(this, ref(OnVersion));
	events.onPath      = this->events.onPath;
	events.onParameter = this->events.onQueryParameter;
	events.onHeader    = HTTP_OnHeader_For(this, ref(OnHeader));

	HTTP_Header header;
	HTTP_Header_Init(&header, events);
	HTTP_Header_Parse(&header, HTTP_Header_Type_Request,
		String_Slice(this->header.rd, 0, requestOffset));

	if (this->headers.contentLength > 0) {
		/* The request has a body. */
		this->body = String_New(this->headers.contentLength);

		/* It is likely that we already have fetched some body chunks in our header buffer. */
		if (this->header.len != (size_t) requestOffset) {
			if (this->header.len - requestOffset >= this->headers.contentLength) {
				/* We have the whole body in this->header. */
				String_Copy(&this->body,
					String_Slice(this->header.rd, requestOffset, this->headers.contentLength));

				/* In this->header there is more data which does not belong to the body.
				 * Probably it's already the next request.
				 */
				String_Crop(&this->header, requestOffset + this->headers.contentLength);
			} else {
				/* The body is only partial. */
				String_Copy(&this->body,
					String_Slice(this->header.rd, requestOffset));

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
		 * If this->header does not hold anything except the current request,
		 * this empties it.
		 */

		String_Crop(&this->header, requestOffset);
		this->state = ref(State_Dispatch);
	}

	return ref(Result_Complete);
}

def(ref(Result), ReadBody) {
	if (this->headers.contentType == HTTP_ContentType_MultiPart) {
		/* TODO */
	} else {
		/* Get the remaining chunks (if any). */
		while (String_GetFree(this->body) > 0) {
			ssize_t len = SocketConnection_Read(this->conn,
				this->body.buf + this->body.len,
				String_GetFree(this->body));

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
				HTTP_Query_Decode(&qry, this->body.rd, true);
			}
		} finally {
			this->state = ref(State_Header);
		} tryEnd;
	}

	this->state = ref(State_Dispatch);

	return ref(Result_Complete);
}

def(bool, Dispatch) {
	callback(this->events.onRespond, this->headers.persistentConnection);
	this->state = ref(State_Header);

	if (this->header.len == 0) {
		/* We're not aware of any upcoming requests. Thus, Process() must be
		 * called manually as soon as epoll notifies about new data on the
		 * socket.
		 */
		return false;
	}

	/* We already have the next request in our header buffer. Process it right
	 * now.
	 */
	return call(Process);
}

/* Call this method when you're sure that there is some data available on the
 * socket. Otherwise an `OnRequest' event will be triggered in ReadHeader() even
 * though recv() might have returned with EAGAIN.
 *
 * Returns true when more data is required.
 */
def(bool, Process) {
	ref(Result) res = ref(Result_Error);

	if (this->state == ref(State_Header)) {
		res = call(ReadHeader);
	} else if (this->state == ref(State_Body)) {
		res = call(ReadBody);
	} else if (this->state == ref(State_Dispatch)) {
		return call(Dispatch);
	}

	if (res == ref(Result_Complete)) {
		/* Try to handle the next state now. */
		return call(Process);
	}

	/* Keep the connection open if more data is required. */
	return res == ref(Result_Incomplete);
}
