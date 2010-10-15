#import "Header.h"

#import "../Array.h"
#import "../String.h"
#import "../Socket.h"
#import "../StringArray.h"
#import "../SocketConnection.h"
#import "../ExceptionManager.h"

#undef self
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

typedef struct {
	HTTP_OnVersion onVersion;
	HTTP_OnHeader  onHeader;
	void           *context;
} HTTP_Client_Events;

typedef struct {
	String host;
	String path;
} HTTP_Client_HostPath;

typedef Array(HTTP_Client_HostPath, HTTP_Client_HostPaths);

typedef struct {
	Socket             socket;
	SocketConnection   conn;
	String             host;
	short              port;
	bool               closed;
	HTTP_Client_Events events;
	HTTP_Version       version;

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
	int64_t total;

	/* How many bytes out of `total' have been read so far. */
	int64_t read;

	/* How many bytes of the contents were pre-buffered in `resp'. */
	u64 canRead;

	/* Are we inside a chunk?
	 * If a chunk is fully processed (i.e. `read' >= `total')
	 * and `inChunk' is true, then it is expected that a CRLF
	 * follows (which also marks its end).
	 */
	bool inChunk;
} HTTP_Client;

extern size_t Modules_HTTP_Client;

void HTTP_Client0(ExceptionManager *e);
overload void HTTP_Client_Init(HTTP_Client *this);
overload void HTTP_Client_Init(HTTP_Client *this, String host);
overload void HTTP_Client_Init(HTTP_Client *this, String host, short port);
void HTTP_Client_Destroy(HTTP_Client *this);
overload void HTTP_Client_SetBufferSize(HTTP_Client *this, size_t size);
void HTTP_Client_SetEvents(HTTP_Client *this, HTTP_Client_Events events);
void HTTP_Client_SetVersion(HTTP_Client *this, HTTP_Version version);
overload void HTTP_Client_Open(HTTP_Client *this);
overload void HTTP_Client_Open(HTTP_Client *this, String host);
overload void HTTP_Client_Open(HTTP_Client *this, String host, short port);
void HTTP_Client_Close(HTTP_Client *this);
void HTTP_Client_OnStatus(HTTP_Client *this, HTTP_Status status);
void HTTP_Client_OnVersion(HTTP_Client *this, HTTP_Version version);
void HTTP_Client_OnHeader(HTTP_Client *this, String name, String value);
int64_t HTTP_Client_GetLength(HTTP_Client *this);
bool HTTP_Client_IsConnected(HTTP_Client *this);
void HTTP_Client_Reopen(HTTP_Client *this);
String HTTP_Client_GetRequest(HTTP_Client *this, String host, String path);
overload void HTTP_Client_Request(HTTP_Client *this, HTTP_Client_HostPaths *items);
overload void HTTP_Client_Request(HTTP_Client *this, StringArray paths);
overload void HTTP_Client_Request(HTTP_Client *this, String host, String path);
overload void HTTP_Client_Request(HTTP_Client *this, String path);
size_t HTTP_Client_ParseChunk(String *s);
void HTTP_Client_ProcessChunk(HTTP_Client *this);
HTTP_Status HTTP_Client_FetchResponse(HTTP_Client *this);
overload bool HTTP_Client_Read(HTTP_Client *this, String *res);
overload String HTTP_Client_Read(HTTP_Client *this, size_t max);
