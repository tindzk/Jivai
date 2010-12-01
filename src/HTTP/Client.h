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

enum {
	excConnectionError = excOffset,
	excMalformedChunk,
	excResponseMalformed
};

record(ref(Events)) {
	HTTP_OnVersion onVersion;
	HTTP_OnHeader  onHeader;
};

record(ref(HostPath)) {
	String host;
	String path;
};

typedef Array(ref(HostPath), ref(HostPaths));

class {
	Socket           socket;
	SocketConnection conn;
	String           host;
	short            port;
	bool             closed;
	ref(Events)      events;
	HTTP_Version     version;

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

overload def(void, Init);
overload def(void, Init, String host);
overload def(void, Init, String host, short port);
def(void, Destroy);
overload def(void, SetBufferSize, size_t size);
def(void, SetEvents, ref(Events) events);
def(void, SetVersion, HTTP_Version version);
overload def(void, Open);
overload def(void, Open, String host);
overload def(void, Open, String host, short port);
def(void, Close);
def(void, OnStatus, HTTP_Status status);
def(void, OnVersion, HTTP_Version version);
def(void, OnHeader, String name, String value);
def(s64, GetLength);
def(bool, IsConnected);
def(void, Reopen);
def(String, GetRequest, String host, String path);
overload def(void, Request, ref(HostPaths) *items);
overload def(void, Request, StringArray paths);
overload def(void, Request, String host, String path);
overload def(void, Request, String path);
sdef(s64, ParseChunk, String *s);
def(void, ProcessChunk);
def(HTTP_Status, FetchResponse);
overload def(bool, Read, String *res);
overload def(String, Read, size_t max);

#define HTTP_Client_Init(obj, ...) \
	HTTP_Client_Init(HTTP_Client_FromObject(obj), ## __VA_ARGS__)

#define HTTP_Client_Open(obj, ...) \
	HTTP_Client_Open(HTTP_Client_FromObject(obj), ## __VA_ARGS__)

#define HTTP_Client_Request(obj, ...) \
	HTTP_Client_Request(HTTP_Client_FromObject(obj), ## __VA_ARGS__)

#define HTTP_Client_Read(obj, ...) \
	HTTP_Client_Read(HTTP_Client_FromObject(obj), ## __VA_ARGS__)

#undef self
