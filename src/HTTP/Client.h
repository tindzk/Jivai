#import "Header.h"

#import "../Array.h"
#import "../String.h"
#import "../Socket.h"
#import "../Exception.h"
#import "../SocketConnection.h"

#define self HTTP_Client

/* Will be mostly used for buffering the headers.
 * Therefore, it shouldn't be too large.
 */
#ifndef HTTP_Client_BufferSize
#define HTTP_Client_BufferSize 4096
#endif

/* For HTTP_Client_Read() only. The larger the value,
 * the fewer syscalls are needed but the more memory
 * is allocated.
 */
#ifndef HTTP_Client_ReadChunkSize
#define HTTP_Client_ReadChunkSize 16384
#endif

// @exc BufferTooSmall
// @exc ConnectionError
// @exc ConnectionReset
// @exc MalformedChunk
// @exc ResponseMalformed

record(ref(RequestItem)) {
	HTTP_Method   method;
	HTTP_Version  version;
	CarrierString host;
	CarrierString path;
	String        *data;
};

Array(ref(RequestItem), ref(Requests));

class {
	Socket           socket;
	SocketConnection conn;
	CarrierString    host;
	short            port;
	bool             closed;

	/* Events. */
	HTTP_OnResponseInfo onResponseInfo;
	HTTP_OnHeader       onHeader;

	/* The resolved response code. */
	HTTP_Status status;

	/* Buffer for headers. Might already contain parts
	 * of the body.
	 */
	String resp;

	/* True, if the `Transfer-Encoding' header is set to
	 * `chunked'.
	 */
	bool chunked;

	/* By default true for HTTP/1.1, but disabled for
	 * HTTP/1.0. Clients can explicitly enable/disable
	 * Keep-Alive connections employing the `Connection'
	 * header.
	 */
	bool keepAlive;

	/* As for non-chunked responses, this is the total
	 * length (supplied with the `Content-Length' header)
	 * whereas for chunked responses, it's the length
	 * of the current chunk solely.
	 */
	s64 total;

	/* How many bytes out of `total' have been read so far. */
	s64 read;

	/* How many bytes of the contents were pre-buffered in `resp'. */
	u64 canRead;

	/* Are we inside a chunk?
	 * If a chunk is fully processed (i.e. `read' >= `total')
	 * and `inChunk' is true, then it is expected that a CRLF
	 * follows (which also marks its end).
	 */
	bool inChunk;
};

sdef(ref(RequestItem), CreateRequest, CarrierString host, CarrierString path);
overload def(void, Init);
overload def(void, Init, CarrierString host);
overload def(void, Init, CarrierString host, short port);
def(void, Destroy);
overload def(void, SetBufferSize, size_t size);
def(void, BindResponseInfo, HTTP_OnResponseInfo onResponseInfo);
def(void, BindHeader, HTTP_OnHeader onHeader);
overload def(void, Open);
overload def(void, Open, CarrierString host);
overload def(void, Open, CarrierString host, short port);
def(void, Close);
def(HTTP_Status_Item, GetStatus);
def(s64, GetLength);
def(bool, IsConnected);
def(void, Reopen);
overload def(void, Request, ref(Requests) *items);
overload def(void, Request, ref(RequestItem) request);
sdef(s64, ParseChunk, String *s);
def(void, ProcessChunk);
def(HTTP_Status, FetchResponse);
overload def(bool, Read, String *res);
overload def(String, Read, size_t max);

#undef self
