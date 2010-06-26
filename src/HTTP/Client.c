#include "Client.h"

static ExceptionManager *exc;

Exception_Define(HTTP_Client_BufferTooSmallException);
Exception_Define(HTTP_Client_ConnectionResetException);
Exception_Define(HTTP_Client_MalformedChunkException);
Exception_Define(HTTP_Client_ResponseMalformedException);

void HTTP_Client0(ExceptionManager *e) {
	exc = e;
}

void OVERLOAD HTTP_Client_Init(HTTP_Client *this) {
	this->closed = true;

	this->resp = HeapString(HTTP_Client_BufferSize);

	this->host = HeapString(0);
	this->port = 80;

	this->events.onVersion = NULL;
	this->events.onHeader  = NULL;
	this->events.context   = NULL;

	Socket_Init(&this->socket, Socket_Protocol_TCP);
}

void OVERLOAD HTTP_Client_Init(HTTP_Client *this, String host) {
	HTTP_Client_Init(this);

	this->host = String_Clone(host);
}

void OVERLOAD HTTP_Client_Init(HTTP_Client *this, String host, short port) {
	HTTP_Client_Init(this);

	this->host = String_Clone(host);
	this->port = port;
}

void HTTP_Client_Destroy(HTTP_Client *this) {
	String_Destroy(&this->host);
	String_Destroy(&this->resp);

	if (!this->closed) {
		HTTP_Client_Close(this);
	}

	Socket_Destroy(&this->socket);
}

void OVERLOAD HTTP_Client_SetBufferSize(HTTP_Client *this, size_t size) {
	String_Align(&this->resp, size);
}

void HTTP_Client_SetEvents(HTTP_Client *this, HTTP_Client_Events events) {
	this->events = events;
}

void OVERLOAD HTTP_Client_Open(HTTP_Client *this) {
	this->conn = Socket_Connect(&this->socket, this->host, this->port);
	this->closed = false;
}

void OVERLOAD HTTP_Client_Open(HTTP_Client *this, String host) {
	String_Copy(&this->host, host);
	HTTP_Client_Open(this);
}

void OVERLOAD HTTP_Client_Open(HTTP_Client *this, String host, short port) {
	String_Copy(&this->host, host);
	this->port = port;

	HTTP_Client_Open(this);
}

void HTTP_Client_Close(HTTP_Client *this) {
	this->closed = true;
	SocketConnection_Close(&this->conn);
}

void HTTP_Client_OnStatus(HTTP_Client *this, HTTP_Status status) {
	this->status = status;
}

void HTTP_Client_OnVersion(HTTP_Client *this, HTTP_Version version) {
	if (this->events.onVersion != NULL) {
		this->events.onVersion(this->events.context, version);
	}

	if (version == HTTP_Version_1_1) {
		this->keepAlive = true;
	} else if (version == HTTP_Version_1_0) {
		this->keepAlive = false;
	}
}

void HTTP_Client_OnHeader(HTTP_Client *this, String name, String value) {
	if (this->events.onHeader != NULL) {
		this->events.onHeader(this->events.context, name, value);
	}

	String_ToLower(&name);

	if (String_Equals(&name, String("connection"))) {
		String_ToLower(&value);

		if (String_Equals(&value, String("close"))) {
			this->keepAlive = false;
		} else if (String_Equals(&value, String("keep-alive"))) {
			this->keepAlive = true;
		}
	} else if (String_Equals(&name, String("transfer-encoding"))) {
		if (String_Equals(&value, String("chunked"))) {
			this->chunked = true;
		}
	} else if (String_Equals(&name, String("content-length"))) {
		this->total = Integer64_ParseString(value);
	}
}

void HTTP_Client_Reopen(HTTP_Client *this) {
	if (this->closed) {
		HTTP_Client_Open(this);
	}
}

String HTTP_Client_GetRequest(String host, String path) {
	String res = String_Format(String(
		"GET % HTTP/1.1\r\n"
		"Host: %\r\n"
		"Connection: Keep-Alive\r\n"
		"\r\n"),

		path, host);

	return res;
}

void OVERLOAD HTTP_Client_Request(HTTP_Client *this, HTTP_Client_HostPaths items) {
	HTTP_Client_Reopen(this);

	String s = HeapString(items.len * 50);

	for (size_t i = 0; i < items.len; i++) {
		String tmp = HTTP_Client_GetRequest(items.buf[i].host, items.buf[i].path);
		String_Append(&s, tmp);
		String_Destroy(&tmp);
	}

	try (exc) {
		SocketConnection_Write(&this->conn, s.buf, s.len);
	} finally {
		String_Destroy(&s);
	} tryEnd;
}

void OVERLOAD HTTP_Client_Request(HTTP_Client *this, StringArray paths) {
	HTTP_Client_Reopen(this);

	String s = HeapString(paths.len * 50);

	for (size_t i = 0; i < paths.len; i++) {
		String tmp = HTTP_Client_GetRequest(this->host, paths.buf[i]);
		String_Append(&s, tmp);
		String_Destroy(&tmp);
	}

	try (exc) {
		SocketConnection_Write(&this->conn, s.buf, s.len);
	} finally {
		String_Destroy(&s);
	} tryEnd;
}

void OVERLOAD HTTP_Client_Request(HTTP_Client *this, String host, String path) {
	HTTP_Client_Reopen(this);

	String request = HTTP_Client_GetRequest(host, path);

	try (exc) {
		SocketConnection_Write(&this->conn, request.buf, request.len);
	} finally {
		String_Destroy(&request);
	} tryEnd;
}

void OVERLOAD HTTP_Client_Request(HTTP_Client *this, String path) {
	HTTP_Client_Request(this, this->host, path);
}

size_t HTTP_Client_ParseChunk(String *s) {
	ssize_t pos = String_Find(s, String("\r\n"));

	if (pos == String_NotFound) {
		throw(exc, &HTTP_Client_MalformedChunkException);
	}

	String part = String_Slice(s, 0, pos);
	String_Crop(s, pos + 2);

	long len = Hex_ToInteger(part);
	String_Destroy(&part);

	return len;
}

void HTTP_Client_ProcessChunk(HTTP_Client *this) {
	if (this->resp.len > 0) { /* Might already contain bits of the next chunk. */
		this->total = HTTP_Client_ParseChunk(&this->resp);

		if (this->resp.len >= this->total) {
			/* We got the complete chunk. Perhaps even more than that. */
			this->canRead = this->total;
		} else {
			this->canRead = this->resp.len;
		}
	}
}

HTTP_Status HTTP_Client_FetchResponse(HTTP_Client *this) {
	/* Reset the values which were extracted from previous requests' headers. */
	this->total     = -1;
	this->chunked   = false;
	this->keepAlive = false;
	this->resp.len  = 0;

	try (exc) {
		this->resp.len += SocketConnection_Read(&this->conn,
			this->resp.buf  + this->resp.len,
			this->resp.size - this->resp.len);

		/* See HTTP/Server.c for a complete explanation how all this works. */
		ssize_t requestOffset = HTTP_Header_GetLength(this->resp);

		if (requestOffset == -1) { /* The response is malformed. */
			this->resp.len = 0;
			this->closed   = true;

			throw(exc, &HTTP_Client_ResponseMalformedException);
		} else if (requestOffset > 0) { /* The response is complete. */
			HTTP_Header_Events events;
			events.context   = this;
			events.onVersion = (void *) &HTTP_Client_OnVersion;
			events.onStatus  = (void *) &HTTP_Client_OnStatus;
			events.onHeader  = (void *) &HTTP_Client_OnHeader;

			String s = String_FastSlice(&this->resp, 0, requestOffset);

			HTTP_Header header;
			HTTP_Header_Init(&header, events);
			HTTP_Header_Parse(&header, HTTP_Header_Type_Response, s);

			String_Crop(&this->resp, requestOffset);

			/* Some HTTP servers send header and body separately.
			 * The first chunk is needed, though. Otherwise
			 * HTTP_Client_Read() will fail right away.
			 */
			if (this->resp.len == 0) {
				this->resp.len = SocketConnection_Read(&this->conn,
					this->resp.buf,
					this->resp.size);
			}
		} else { /* Response is incomplete. */
			if (this->resp.len == this->resp.size) {
				/* But buffer is already full, i.e. we cannot store
				 * more in it. Increasing the buffer size might
				 * help. If not, the HTTP server sends corrupt
				 * responses.
				 */

				throw(exc, &HTTP_Client_BufferTooSmallException);
			}
		}
	} catch(&SocketConnection_ConnectionResetException, e) {
		this->closed = true;
	} finally {

	} tryEnd;

	if (this->chunked) {
		this->inChunk = true;
		HTTP_Client_ProcessChunk(this); /* Sets total and canRead appropriately. */
	} else {
		this->inChunk = false;
		this->canRead = this->resp.len;
	}

	this->read = 0;

	return this->status;
}

bool OVERLOAD HTTP_Client_Read(HTTP_Client *this, String *res) {
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
				HTTP_Client_Close(this);
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
			String_Copy(res, this->resp);

			this->read += this->resp.len;

			this->canRead  = 0;
			this->resp.len = 0;
		}
	}

	while (true) {
		if (this->total != -1 && this->read >= this->total) {
			if (this->chunked) {
				/* Chunk is complete. */
				if (this->resp.size > this->resp.len) {
					try (exc) {
						size_t read = SocketConnection_Read(&this->conn,
							this->resp.buf  + this->resp.len,
							this->resp.size - this->resp.len);

						this->resp.len += read;
					} catch(&SocketConnection_ConnectionResetException, e) {
						this->closed = true;
						throw(exc, &HTTP_Client_ConnectionResetException);
					} finally {

					} tryEnd;
				}

				if (this->resp.len > 0) {
					if (this->inChunk) {
						if (!String_BeginsWith(&this->resp, String("\r\n"))) {
							/* Chunk does not end on CRLF. */
							throw(exc, &HTTP_Client_MalformedChunkException);
						} else {
							String_Crop(&this->resp, 2);

							/* Don't set this->total and this->read to 0 because
							 * this will cause the next HTTP_Client_Read() call
							 * to return true. Just stick with the current values
							 * even though are not valid anymore.
							 */
							this->inChunk = false;

							/* It is possible that `resp' includes only parts of
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
							 * HTTP_Client_ConnectionResetException exception.
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

							if (String_Find(&this->resp, 3, String("\r\n")) != String_NotFound) {
								/* We have enough data to deal with it.
								 * Fall through.
								 */
							} else {
								continue;
							}
						}
					}

					/* Sic. Do not merge this with the above if-statement. */
					if (!this->inChunk) {
						HTTP_Client_ProcessChunk(this);

						this->read    = 0;
						this->inChunk = true;
					}
				}
			}

			return true;
		}

		if (res->len >= res->size) {
			/* Buffer is full. */
			return true;
		}

		try (exc) {
			size_t read;

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

			res->len   += read;
			this->read += read;
		} catch(&SocketConnection_ConnectionResetException, e) {
			this->closed = true;

			if (this->total == -1) {
				/* When no `Content-Length' is set, the server
				 * won't have other options to indicate the end
				 * of a transfer than to close the connection.
				 * That's why no exception will be thrown here.
				 */

				return true;
			} else {
				throw(exc, &HTTP_Client_ConnectionResetException);
			}
		} finally {

		} tryEnd;
	}

	return true;
}

String OVERLOAD HTTP_Client_Read(HTTP_Client *this, size_t max) {
	String res = HeapString(max);

	String buf = HeapString(HTTP_Client_ReadChunkSize);

	while (HTTP_Client_Read(this, &buf)) {
		if (res.len + buf.len > res.size) {
			/* If the response buffer is full, skip the remaining
			 * bytes. Otherwise problems will arise when another
			 * request is waiting to be processed (HTTP pipelining).
			 * Otherwise, HTTP_Client_FetchResponse() will throw
			 * an exception claiming it couldn't find any headers.
			 */
			continue;
		}

		String_Append(&res, buf);
	}

	String_Destroy(&buf);

	return res;
}
