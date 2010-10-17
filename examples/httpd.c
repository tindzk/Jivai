/*
 * Simple web server.
 *
 * Supports persistent connections as well as pipelining.
 * It is compatible to HTTP/1.0 and HTTP/1.1.
 *
 * If you want to use this code in production systems, you
 * should consider including proper error checking (in
 * particular for `HTTP_Request' and `HTTP_Header'
 * exceptions). Otherwise, the server will most likely
 * crash on malformed requests.
 */

#import <Client.h>
#import <Signal.h>
#import <Server.h>
#import <Socket.h>
#import <Integer.h>
#import <Connection.h>
#import <HTTP/Method.h>
#import <HTTP/Server.h>
#import <ClientListener.h>
#import <SocketConnection.h>
#import <DoublyLinkedList.h>
#import <ConnectionInterface.h>

ExceptionManager exc;

// -------
// Request
// -------

typedef struct {
	bool gotData;
	bool persistent;

	HTTP_Server  server;
	HTTP_Method  method;
	HTTP_Version version;

	SocketConnection *conn;

	String resp;
	String path;

	String paramTest;
	String paramTest2;
} Request;

void Request_OnHttpVersion(Request *this, HTTP_Version version) {
	this->version = version;
}

void Request_OnMethod(Request *this, HTTP_Method method) {
	this->method = method;
}

void Request_OnPath(Request *this, String path) {
	String_Copy(&this->path, path);
}

/*
 * These parameters are supplied with the URL, such as:
 * http://localhost:8080/?test=abc&test2=def
 */

String* Request_OnQueryParameter(Request *this, String name) {
	if (String_Equals(name, String("test"))) {
		/* test's value will be put into this variable. */
		return &this->paramTest;
	} else if (String_Equals(name, String("test2"))) {
		return &this->paramTest2;
	}

	/* Ignore all other parameters */
	return NULL;
}

/*
 * Treat body-submitted parameters equally like URL-supplied ones.
 */

String* Request_OnBodyParameter(Request *this, String name) {
	return Request_OnQueryParameter(this, name);
}

void Request_OnRespond(Request *this, bool persistent) {
	this->resp.len = 0;

	if (this->method == HTTP_Method_Get) {
		String_Append(&this->resp, String("Page requested via GET."));
	} else if (this->method == HTTP_Method_Post) {
		String_Append(&this->resp, String("Page requested via POST."));
	} else if (this->method == HTTP_Method_Head) {
		String_Append(&this->resp, String("Page requested via HEAD."));
	}

	if (this->method != HTTP_Method_Head) {
		String tmp;
		String_Append(&this->resp, tmp = String_Format(
			String(
				"<form action=\"/\" method=\"post\">"
				"<input type=\"text\" name=\"test\" /><br />"
				"<input type=\"text\" name=\"test2\" /><br />"
				"<input type=\"submit\" />"
				"</form>"
				"<br />"
				"You requested: %<br />"
				"test=%<br />"
				"test2=%<br />"
			),
			this->path, this->paramTest, this->paramTest2));

		String_Destroy(&tmp);
	}

	String envelope = String_Format(
		String(
			"HTTP/% 200 OK\r\n"
			"Content-Type: text/html; charset=utf-8\r\n"
			"%\r\n"
			"Content-Length: %\r\n"
			"\r\n"
		),

		(this->version == HTTP_Version_1_1) ? String("1.1") : String("1.0"),

		persistent
			? String("Connection: Keep-Alive")
			: String("Connection: Close"),

		Integer64_ToString(this->resp.len));

	SocketConnection_Write(this->conn, envelope.buf, envelope.len);

	size_t written = SocketConnection_Write(this->conn, this->resp.buf, this->resp.len);
	String_Crop(&this->resp, written);

	SocketConnection_Flush(this->conn);

	String_Destroy(&envelope);

	this->paramTest.len  = 0;
	this->paramTest2.len = 0;

	/* You may want to change the value of this variable when the
	 * content length is unknown. If that's the case, you won't have
	 * no choice to indicate the end of the response but to close
	 * the connection.
	 */
	this->persistent = persistent;
}

void Request_Init(Request *this, SocketConnection *conn) {
	this->conn       = conn;
	this->resp       = HeapString(2048);
	this->method     = HTTP_Method_Get;
	this->version    = HTTP_Version_1_0;
	this->gotData    = false;
	this->persistent = false;
	this->path       = HeapString(0);

	/* paramTest and paramTest2 must not be longer than 256 bytes,
	 * otherwise an `HTTP_Query_ExceedsPermittedLengthException'
	 * exception will be thrown.
	 */

	this->paramTest  = HeapString(256);
	this->paramTest2 = HeapString(256);

	HTTP_Server_Events events;
	events.context          = this;
	events.onHeader         = NULL;
	events.onVersion        = (void *) &Request_OnHttpVersion;
	events.onMethod         = (void *) &Request_OnMethod;
	events.onPath           = (void *) &Request_OnPath;
	events.onQueryParameter = (void *) &Request_OnQueryParameter;
	events.onBodyParameter  = (void *) &Request_OnBodyParameter;
	events.onRespond        = (void *) &Request_OnRespond;

	HTTP_Server_Init(&this->server, events, conn, 2048, 4096);
}

void Request_Destroy(Request *this) {
	HTTP_Server_Destroy(&this->server);

	String_Destroy(&this->resp);
	String_Destroy(&this->path);
	String_Destroy(&this->paramTest);
	String_Destroy(&this->paramTest2);
}

Connection_Status Request_Parse(Request *this) {
	this->gotData = true;

	bool incomplete = HTTP_Server_Process(&this->server);

	/* Whilst the request is incomplete, keep the connection alive. */
	if (incomplete) {
		return Connection_Status_Open;
	}

	/* There is enough data but does the current request actually
	 * support persistent connections?
	 */
	return this->persistent
		? Connection_Status_Open
		: Connection_Status_Close;
}

Connection_Status Request_Respond(Request *this) {
	/* In edge-triggered mode a new connection may start with a
	 * `pull' request.
	 *
	 * However, as this->persistent is false, the connection would
	 * be closed right away without even receiving any data.
	 * This is prevented by ignoring the event.
	 */
	if (!this->gotData) {
		return Connection_Status_Open;
	}

	if (this->resp.len > 0) {
		size_t written = SocketConnection_Write(this->conn, this->resp.buf, this->resp.len);
		String_Crop(&this->resp, written);

		bool incomplete = (this->resp.len > 0);

		if (incomplete) {
			return Connection_Status_Open;
		}
	}

	return this->persistent
		? Connection_Status_Open
		: Connection_Status_Close;
}

// --------------
// HttpConnection
// --------------

typedef struct _HttpConnection {
	Connection_Define(_HttpConnection);
	Request request;
} HttpConnection;

HttpConnection* HttpConnection_New(void) {
	return New(HttpConnection);
}

void HttpConnection_Init(HttpConnection *this, SocketConnection *conn) {
	Request_Init(&this->request, conn);
}

void HttpConnection_Destroy(HttpConnection *this) {
	Request_Destroy(&this->request);
}

Connection_Status HttpConnection_Push(HttpConnection *this) {
	return Request_Parse(&this->request);
}

Connection_Status HttpConnection_Pull(HttpConnection *this) {
	return Request_Respond(&this->request);
}

ConnectionInterface HttpConnection_Methods = {
	.new     = (void *) HttpConnection_New,
	.init    = (void *) HttpConnection_Init,
	.destroy = (void *) HttpConnection_Destroy,
	.push    = (void *) HttpConnection_Push,
	.pull    = (void *) HttpConnection_Pull
};

// ----
// main
// ----

bool startServer(Server *server, ClientListener *listener) {
	Server_Events events;

	ClientListener_Init(listener, &HttpConnection_Methods, &events);

	try (&exc) {
		Server_Init(server, events, true, 8080);
		String_Print(String("Server started.\n"));
		excReturn true;
	} clean catch (Modules_Socket, excAddressInUse, e) {
		String_Print(String("The address is already in use!\n"));
		excReturn false;
	} finally {

	} tryEnd;

	return false;
}

int main(void) {
	ExceptionManager_Init(&exc);

	Poll0(&exc);
	String0(&exc);
	Socket0(&exc);
	Memory0(&exc);
	Signal0(&exc);
	Server0(&exc);
	HTTP_Query0(&exc);
	HTTP_Header0(&exc);
	HTTP_Server0(&exc);
	ClientListener0(&exc);
	SocketConnection0(&exc);

	Server         server;
	ClientListener listener;

	if (!startServer(&server, &listener)) {
		return ExitStatus_Failure;
	}

	try (&exc) {
		while (true) {
			Server_Process(&server);
		}
	} clean catch (Modules_Signal, excSigInt, e) {
		String_Print(String("Server shutdown.\n"));
	} catchAny (e) {
		Exception_Print(e);

#if Exception_SaveTrace
		Backtrace_PrintTrace(e->trace, e->traceItems);
#endif

		excReturn ExitStatus_Failure;
	} finally {
		Server_Destroy(&server);
	} tryEnd;

	return ExitStatus_Success;
}
