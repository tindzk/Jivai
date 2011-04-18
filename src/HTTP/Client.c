#import "Client.h"

#define self HTTP_Client

sdef(ref(RequestItem), CreateRequest, CarrierString host, CarrierString path) {
	return (ref(RequestItem)) {
		.method  = HTTP_Method_Get,
		.version = HTTP_Version_1_1,
		.host    = host,
		.path    = path,
		.data    = NULL
	};
}

overload def(void, Init) {
	this->status = HTTP_Status_Unset;
	this->closed = true;

	this->resp = String_New(ref(BufferSize));

	this->host = CarrierString_New();
	this->port = 80;

	this->onResponseInfo = HTTP_OnResponseInfo_Empty();
	this->onHeader       = HTTP_OnHeader_Empty();

	this->socket = Socket_New(Socket_Protocol_TCP);
}

overload def(void, Init, CarrierString host) {
	call(Init);

	this->host = host;
}

overload def(void, Init, CarrierString host, short port) {
	call(Init);

	this->host = host;
	this->port = port;
}

def(void, Destroy) {
	CarrierString_Destroy(&this->host);
	String_Destroy(&this->resp);

	if (!this->closed) {
		call(Close);
	}

	Socket_Destroy(&this->socket);
}

overload def(void, SetBufferSize, size_t size) {
	String_Align(&this->resp, size);
}

def(void, BindResponseInfo, HTTP_OnResponseInfo onResponseInfo) {
	this->onResponseInfo = onResponseInfo;
}

def(void, BindHeader, HTTP_OnHeader onHeader) {
	this->onHeader = onHeader;
}

overload def(void, Open) {
	if (!this->closed) {
		call(Close);
	}

	this->conn = Socket_Connect(&this->socket, this->host.rd, this->port);
	this->closed = false;
}

overload def(void, Open, CarrierString host) {
	CarrierString_Assign(&this->host, host);
	call(Open);
}

overload def(void, Open, CarrierString host, short port) {
	CarrierString_Assign(&this->host, host);
	this->port = port;

	call(Open);
}

def(void, Close) {
	this->closed = true;
	SocketConnection_Close(&this->conn);
}

def(HTTP_Status_Item, GetStatus) {
	return HTTP_Status_GetItem(this->status);
}

static def(void, OnResponseInfo, HTTP_ResponseInfo info) {
	this->status = info.status;

	if (info.version == HTTP_Version_1_1) {
		this->keepAlive = true;
	} else if (info.version == HTTP_Version_1_0) {
		this->keepAlive = false;
	}

	callback(this->onResponseInfo, info);
}

static def(void, OnHeader, RdString name, RdString value) {
	callback(this->onHeader, name, value);

	String_ToLower((String *) &name);

	if (String_Equals(name, $("connection"))) {
		String_ToLower((String *) &value);

		if (String_Equals(value, $("close"))) {
			this->keepAlive = false;
		} else if (String_Equals(value, $("keep-alive"))) {
			this->keepAlive = true;
		}
	} else if (String_Equals(name, $("transfer-encoding"))) {
		if (String_Equals(value, $("chunked"))) {
			this->chunked = true;
		}
	} else if (String_Equals(name, $("content-length"))) {
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

static def(void, _CreateRequest, ref(RequestItem) request, String *res) {
	String fmt = String_Format($(
		"% % %\r\n"
		"Host: %\r\n"
		"Connection: Keep-Alive\r\n"),

		HTTP_Method_ToString(request.method),
		request.path.rd,
		HTTP_Version_ToString(request.version),
		request.host.rd);

	String_Append(res, fmt.rd);

	String_Destroy(&fmt);

	if (request.method == HTTP_Method_Post) {
		if (request.data == NULL) {
			String_Append(res, $("Content-Length: 0\r\n"));
		} else {
			String_Append(res,
				$("Content-Type: application/x-www-form-urlencoded\r\n"));

			String strLength = Integer_ToString(request.data->len);

			String_Append(res, $("Content-Length: "));
			String_Append(res, strLength.rd);
			String_Append(res, $("\r\n"));
			String_Append(res, request.data->rd);

			String_Destroy(&strLength);
		}
	}

	String_Append(res, $("\r\n"));
}

overload def(void, Request, ref(Requests) *items) {
	call(Reopen);

	String s = String_New(items->len * 128);

	fwd(i, items->len) {
		call(_CreateRequest, items->buf[i], &s);
	}

	try {
		SocketConnection_Write(&this->conn, s.buf, s.len);
	} finally {
		String_Destroy(&s);
	} tryEnd;
}

overload def(void, Request, ref(RequestItem) request) {
	call(Reopen);

	String s = String_New(128);

	call(_CreateRequest, request, &s);

	try {
		SocketConnection_Write(&this->conn, s.buf, s.len);
	} finally {
		String_Destroy(&s);
	} tryEnd;
}

sdef(s64, ParseChunk, String *s) {
	ssize_t pos = String_Find(s->rd, $("\r\n"));

	if (pos == String_NotFound) {
		throw(MalformedChunk);
	}

	RdString hex =
		String_Trim(
			String_Slice(s->rd, 0, pos));

	s64 len = Hex_ToInteger(hex);

	if (len == -1) {
		throw(MalformedChunk);
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

	try {
		ssize_t len = SocketConnection_Read(&this->conn,
			this->resp.buf + this->resp.len,
			String_GetFree(this->resp));

		/* SocketConnection_Read() only returns -1 for non-blocking
		 * connections.
		 * For now this module only supports blocking connections.
		 */
		if (len == -1) {
			throw(ConnectionError);
		}

		this->resp.len += len;

		/* See HTTP/Server.c for a complete explanation how all this works. */
		ssize_t requestOffset = HTTP_Header_GetLength(this->resp.rd);

		if (requestOffset == -1) { /* The response is malformed. */
			this->resp.len = 0;
			this->closed   = true;

			throw(ResponseMalformed);
		} else if (requestOffset > 0) { /* The response is complete. */
			HTTP_Header_Events events = {
				.onResponseInfo = HTTP_OnResponseInfo_For(this, ref(OnResponseInfo)),
				.onHeader       = HTTP_OnHeader_For(this, ref(OnHeader))
			};

			RdString s = String_Slice(this->resp.rd, 0, requestOffset);

			HTTP_Header header = HTTP_Header_New(events);
			HTTP_Header_ParseResponse(&header, s);

			String_Crop(&this->resp, requestOffset);

			/* Some HTTP servers send header and body separately.
			 * The first chunk is needed, though. Otherwise Read()
			 * will fail right away.
			 */
			if (this->resp.len == 0 && this->total != 0) {
				len = SocketConnection_Read(&this->conn,
					this->resp.buf,
					String_GetFree(this->resp));

				if (len == -1) {
					throw(ConnectionError);
				}

				this->resp.len = len;
			}
		} else { /* Response is incomplete. */
			if (String_GetFree(this->resp) == 0) {
				/* But buffer is already full, i.e. we cannot store
				 * more in it. Increasing the buffer size might
				 * help. If not, the HTTP server sends corrupt
				 * responses.
				 */

				throw(BufferTooSmall);
			}
		}
	} catch(SocketConnection, ConnectionReset) {
		this->closed = true;
		excThrow(ConnectionReset);
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
	try {
		ssize_t len = SocketConnection_Read(&this->conn,
			this->resp.buf + this->resp.len,
			String_GetFree(this->resp));

		if (len == -1) {
			throw(ConnectionError);
		}

		this->resp.len += len;
	} catch(SocketConnection, ConnectionReset) {
		this->closed = true;
		excThrow(ConnectionReset);
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
		if (this->canRead > String_GetSize(*res)) {
			String_Copy(res,
				String_Slice(this->resp.rd, 0, String_GetSize(*res)));

			String_Crop(&this->resp, String_GetSize(*res));

			this->read    += res->len;
			this->canRead -= res->len;

			return true;
		} else {
			String_Copy(res,
				String_Slice(this->resp.rd, 0, this->canRead));

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
				} else if (!String_BeginsWith(this->resp.rd, $("\r\n"))) {
					/* Chunk does not end on CRLF. */
					throw(MalformedChunk);
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
					 * ConnectionReset exception.
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

					if (String_Find(this->resp.rd, 2, $("\r\n")) != String_NotFound) {
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

		if (String_GetFree(*res) == 0) {
			/* Buffer is full. */
			break;
		}

		ssize_t read = 0;

		try {
			if (this->chunked) {
				/* Use the full available space. */
				size_t length = String_GetFree(*res);

				/* But it mustn't exceed the number of remaining bytes
				 * in the current chunk. */
				if (length > this->total - this->read) {
					length = this->total - this->read;
				}

				read = SocketConnection_Read(&this->conn, res->buf + res->len, length);
			} else {
				read = SocketConnection_Read(&this->conn,
					res->buf + res->len,
					String_GetFree(*res));
			}

			if (read == -1) {
				throw(ConnectionError);
			}

			res->len   += read;
			this->read += read;
		} catch(SocketConnection, ConnectionReset) {
			this->closed = true;

			if (this->total == -1) {
				/* When no `Content-Length' is set, the server
				 * won't have other options to indicate the end
				 * of a transfer than to close the connection.
				 * That's why no exception will be thrown here.
				 */

				excBreak;
			} else {
				excThrow(ConnectionReset);
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
	String res = String_New(max);

	String buf = String_New(ref(ReadChunkSize));

	while (call(Read, &buf)) {
		if (res.len + buf.len > max) {
			/* If the response buffer is full, skip the remaining
			 * bytes. Otherwise problems will arise when another
			 * request is waiting to be processed (HTTP pipelining).
			 * Otherwise, FetchResponse() will throw an exception
			 * claiming it couldn't find any headers.
			 */
			continue;
		}

		String_Append(&res, buf.rd);
	}

	String_Destroy(&buf);

	return res;
}
