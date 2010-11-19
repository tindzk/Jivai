#import "Client.h"
#import "App.h"

#undef HTTP_Client_Init
#undef HTTP_Client_Open
#undef HTTP_Client_Request
#undef HTTP_Client_Read

static ExceptionManager *exc;

void HTTP_Client0(ExceptionManager *e) {
	exc = e;
}

overload def(void, Init) {
	this->closed = true;

	this->resp = HeapString(ref(BufferSize));

	this->host = HeapString(0);
	this->port = 80;

	this->version = HTTP_Version_1_1;

	this->events.onVersion = (HTTP_OnVersion) EmptyCallback();
	this->events.onHeader  = (HTTP_OnHeader)  EmptyCallback();

	Socket_Init(&this->socket, Socket_Protocol_TCP);
}

overload def(void, Init, String host) {
	call(Init);

	this->host = String_Clone(host);
}

overload def(void, Init, String host, short port) {
	call(Init);

	this->host = String_Clone(host);
	this->port = port;
}

def(void, Destroy) {
	String_Destroy(&this->host);
	String_Destroy(&this->resp);

	if (!this->closed) {
		call(Close);
	}

	Socket_Destroy(&this->socket);
}

overload def(void, SetBufferSize, size_t size) {
	String_Align(&this->resp, size);
}

def(void, SetEvents, ref(Events) events) {
	this->events = events;
}

def(void, SetVersion, HTTP_Version version) {
	this->version = version;
}

overload def(void, Open) {
	if (!this->closed) {
		call(Close);
	}

	this->conn = Socket_Connect(&this->socket, this->host, this->port);
	this->closed = false;
}

overload def(void, Open, String host) {
	String_Copy(&this->host, host);
	call(Open);
}

overload def(void, Open, String host, short port) {
	String_Copy(&this->host, host);
	this->port = port;

	call(Open);
}

def(void, Close) {
	this->closed = true;
	SocketConnection_Close(&this->conn);
}

def(void, OnStatus, HTTP_Status status) {
	this->status = status;
}

def(void, OnVersion, HTTP_Version version) {
	callback(this->events.onVersion, version);

	if (version == HTTP_Version_1_1) {
		this->keepAlive = true;
	} else if (version == HTTP_Version_1_0) {
		this->keepAlive = false;
	}
}

def(void, OnHeader, String name, String value) {
	callback(this->events.onHeader, name, value);

	/* See HTTP/Server.c for a justification of this hack. */
	name.mutable  = true;
	value.mutable = true;

	String_ToLower(&name);

	if (String_Equals(name, String("connection"))) {
		String_ToLower(&value);

		if (String_Equals(value, String("close"))) {
			this->keepAlive = false;
		} else if (String_Equals(value, String("keep-alive"))) {
			this->keepAlive = true;
		}
	} else if (String_Equals(name, String("transfer-encoding"))) {
		if (String_Equals(value, String("chunked"))) {
			this->chunked = true;
		}
	} else if (String_Equals(name, String("content-length"))) {
		this->total = Int64_Parse(value);
	}
}

def(s64, GetLength) {
	return this->total;
}

def(bool, IsConnected) {
	return !this->closed;
}

def(void, Reopen) {
	if (this->closed) {
		call(Open);
	}
}

def(String, GetRequest, String host, String path) {
	String res = String_Format(String(
		"GET % %\r\n"
		"Host: %\r\n"
		"Connection: Keep-Alive\r\n"
		"\r\n"),

		path,

		this->version == HTTP_Version_1_1
			? String("HTTP/1.1")
			: String("HTTP/1.0"),

		host);

	return res;
}

overload def(void, Request, ref(HostPaths) *items) {
	call(Reopen);

	String s = HeapString(items->len * 50);

	for (size_t i = 0; i < items->len; i++) {
		String tmp = call(GetRequest,
			items->buf[i].host, items->buf[i].path);
		String_Append(&s, tmp);
		String_Destroy(&tmp);
	}

	try (exc) {
		SocketConnection_Write(&this->conn, s.buf, s.len);
	} clean finally {
		String_Destroy(&s);
	} tryEnd;
}

overload def(void, Request, StringArray paths) {
	call(Reopen);

	String s = HeapString(paths.len * 50);

	for (size_t i = 0; i < paths.len; i++) {
		String tmp = call(GetRequest, this->host, paths.buf[i]);
		String_Append(&s, tmp);
		String_Destroy(&tmp);
	}

	try (exc) {
		SocketConnection_Write(&this->conn, s.buf, s.len);
	} clean finally {
		String_Destroy(&s);
	} tryEnd;
}

overload def(void, Request, String host, String path) {
	call(Reopen);

	String request = call(GetRequest, host, path);

	try (exc) {
		SocketConnection_Write(&this->conn, request.buf, request.len);
	} clean finally {
		String_Destroy(&request);
	} tryEnd;
}

overload def(void, Request, String path) {
	call(Request, this->host, path);
}

sdef(size_t, ParseChunk, String *s) {
	ssize_t pos = String_Find(*s, String("\r\n"));

	if (pos == String_NotFound) {
		throw(exc, excMalformedChunk);
	}

	s64 len = Hex_ToInteger(String_Slice(*s, 0, pos));

	if (len == -1) {
		throw(exc, excMalformedChunk);
	}

	String_Crop(s, pos + 2);

	return len;
}

def(void, ProcessChunk) {
	if (this->resp.len > 0) { /* Might already contain bits of the next chunk. */
		this->total = scall(ParseChunk, &this->resp);

		if (this->resp.len >= this->total) {
			/* We got the complete chunk. Perhaps even more than that. */
			this->canRead = this->total;
		} else {
			this->canRead = this->resp.len;
		}
	}
}

def(HTTP_Status, FetchResponse) {
	/* Reset the values which were extracted from previous requests' headers. */
	this->total     = -1;
	this->chunked   = false;
	this->keepAlive = false;
	this->resp.len  = 0;

	try (exc) {
		ssize_t len = SocketConnection_Read(&this->conn,
			this->resp.buf  + this->resp.len,
			this->resp.size - this->resp.len);

		/* SocketConnection_Read() only returns -1 for non-blocking
		 * connections.
		 * For now this module only supports blocking connections.
		 */
		if (len == -1) {
			throw(exc, excConnectionError);
		}

		this->resp.len += len;

		/* See HTTP/Server.c for a complete explanation how all this works. */
		ssize_t requestOffset = HTTP_Header_GetLength(this->resp);

		if (requestOffset == -1) { /* The response is malformed. */
			this->resp.len = 0;
			this->closed   = true;

			throw(exc, excResponseMalformed);
		} else if (requestOffset > 0) { /* The response is complete. */
			HTTP_Header_Events events;
			events.onVersion = (HTTP_OnVersion) Callback(this, ref(OnVersion));
			events.onStatus  = (HTTP_OnStatus)  Callback(this, ref(OnStatus));
			events.onHeader  = (HTTP_OnHeader)  Callback(this, ref(OnHeader));

			String s = String_Slice(this->resp, 0, requestOffset);

			HTTP_Header header;
			HTTP_Header_Init(&header, events);
			HTTP_Header_Parse(&header, HTTP_Header_Type_Response, s);

			String_Crop(&this->resp, requestOffset);

			/* Some HTTP servers send header and body separately.
			 * The first chunk is needed, though. Otherwise Read()
			 * will fail right away.
			 */
			if (this->resp.len == 0 && this->total != 0) {
				len = SocketConnection_Read(&this->conn,
					this->resp.buf,
					this->resp.size);

				if (len == -1) {
					throw(exc, excConnectionError);
				}

				this->resp.len = len;
			}
		} else { /* Response is incomplete. */
			if (this->resp.len == this->resp.size) {
				/* But buffer is already full, i.e. we cannot store
				 * more in it. Increasing the buffer size might
				 * help. If not, the HTTP server sends corrupt
				 * responses.
				 */

				throw(exc, excBufferTooSmall);
			}
		}
	} clean catch(SocketConnection, excConnectionReset) {
		this->closed = true;
		excThrow(exc, excConnectionReset);
	} finally {

	} tryEnd;

	if (this->chunked) {
		this->inChunk = true;
		call(ProcessChunk); /* Sets total and canRead appropriately. */
	} else {
		this->inChunk = false;
		this->canRead = this->resp.len;
	}

	this->read = 0;

	return this->status;
}

static inline def(void, InternalRead) {
	try (exc) {
		ssize_t len = SocketConnection_Read(&this->conn,
			this->resp.buf  + this->resp.len,
			this->resp.size - this->resp.len);

		if (len == -1) {
			throw(exc, excConnectionError);
		}

		this->resp.len += len;
	} clean catch(SocketConnection, excConnectionReset) {
		this->closed = true;
		excThrow(exc, excConnectionReset);
	} finally {

	} tryEnd;
}

overload def(bool, Read, String *res) {
	res->len = 0;

	if (this->closed) {
		return false;
	}

	/* Sometimes HTTP servers do not send a `Content-Length' header.
	 * Then, `total' will be -1.
	 */
	if (this->total != -1) {
		/* `total' is 0 for the final chunk. */
		if (this->total == 0 || this->read >= this->total) {
			if (!this->keepAlive) {
				call(Close);
			}

			return false;
		}
	}

	/* Process contents of the this->resp buffer first. Can read up to
	 * `canRead' bytes.
	 */
	if (this->canRead > 0) {
		if (this->canRead > res->size) {
			String_Copy(res, this->resp, 0, res->size);
			String_Crop(&this->resp, res->size);

			this->read    += res->len;
			this->canRead -= res->len;

			return true;
		} else {
			String_Copy(res, this->resp, 0, this->canRead);

			/* Do not clear this->resp completely as it might
			 * already contain the next chunk or even more.
			 */
			String_Crop(&this->resp, this->canRead);

			this->read    += res->len;
			this->canRead -= res->len; /* Should be 0 now. */
		}
	}

	while (true) {
		if (this->total != -1 && this->read >= this->total) {
			if (!this->chunked) {
				/* We're done. */
				break;
			}

			if (this->resp.len == 0) {
				call(InternalRead);
			}

		retry:
			if (this->inChunk) { /* Chunk is complete. */
				if (this->resp.len < 2) {
					/* It might happen that this->resp only
					 * contains one character (\r). In this
					 * case checking whether it begins with
					 * \r\n will raise an exception.
					 *
					 * Fetch at least one more byte and then
					 * perform the check below.
					 */

					call(InternalRead);
					goto retry;
				} else if (!String_BeginsWith(this->resp, String("\r\n"))) {
					/* Chunk does not end on CRLF. */
					throw(exc, excMalformedChunk);
				} else {
					/* Don't set this->total and this->read to 0 because
					 * this will cause the next Read() call to
					 * return true. Just stick with the current
					 * values even though are not valid anymore.
					 *
					 * It is possible that `resp' includes only parts of
					 * the chunk identifier (which indicates the chunk's
					 * length). Checking for this->resp.len > 0 will not
					 * work in such cases.
					 *
					 * Checking for \r\n ensures that the chunk identifier
					 * is complete.
					 *
					 * Without this check it can happen that that the final
					 * chunk gets ignored easily. An example from strace
					 * shows the culprit:
					 *
					 * recv(3, "\r\n0\r\n\r\n", 4096, 0)       = 7
					 * recv(3, "", 4091, 0)                    = 0
					 *
					 * It's getting ignored due to the `continue' which tries
					 * to get more data, but since the final tag was already
					 * announced, this will never happen.
					 *
					 * Ultimately, this even leads to the raise of an
					 * excConnectionReset exception.
					 *
					 * Note that this would only happen to the final
					 * chunk, because for the others there's always
					 * enough data available.
					 *
					 * Checking for \r\n is slightly more efficient
					 * because eventually, it reduces the number of
					 * needed syscalls in order to be able to tell
					 * the chunk size. It also requests more data
					 * at once.
					 */

					if (String_Find(this->resp, 2, String("\r\n")) != String_NotFound) {
						/* We have enough data to deal with it.
						 * Fall through.
						 */

						String_Crop(&this->resp, 2);
						this->inChunk = false;
					} else {
						call(InternalRead);
						goto retry;
					}
				}
			}

			/* Sic. Do not merge this with the above if-statement. */
			if (!this->inChunk) {
				call(ProcessChunk);

				this->read    = 0;
				this->inChunk = true;
			}

			break;
		}

		if (res->len >= res->size) {
			/* Buffer is full. */
			break;
		}

		ssize_t read = 0;

		try (exc) {
			if (this->chunked) {
				/* Use the full available space. */
				size_t length = res->size - res->len;

				/* But it mustn't exceed the number of remaining bytes
				 * in the current chunk. */
				if (length > this->total - this->read) {
					length = this->total - this->read;
				}

				read = SocketConnection_Read(&this->conn, res->buf + res->len, length);
			} else {
				read = SocketConnection_Read(&this->conn,
					res->buf  + res->len,
					res->size - res->len);
			}

			if (read == -1) {
				throw(exc, excConnectionError);
			}

			res->len   += read;
			this->read += read;
		} clean catch(SocketConnection, excConnectionReset) {
			this->closed = true;

			if (this->total == -1) {
				/* When no `Content-Length' is set, the server
				 * won't have other options to indicate the end
				 * of a transfer than to close the connection.
				 * That's why no exception will be thrown here.
				 */

				excBreak;
			} else {
				excThrow(exc, excConnectionReset);
			}
		} finally {

		} tryEnd;

		if (read == 0) {
			return false;
		}
	}

	return true;
}

overload def(String, Read, size_t max) {
	String res = HeapString(max);

	String buf = HeapString(ref(ReadChunkSize));

	while (call(Read, &buf)) {
		if (res.len + buf.len > res.size) {
			/* If the response buffer is full, skip the remaining
			 * bytes. Otherwise problems will arise when another
			 * request is waiting to be processed (HTTP pipelining).
			 * Otherwise, FetchResponse() will throw an exception
			 * claiming it couldn't find any headers.
			 */
			continue;
		}

		String_Append(&res, buf);
	}

	String_Destroy(&buf);

	return res;
}
