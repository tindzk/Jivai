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
#import <SocketConnection.h>
#import <DoublyLinkedList.h>
#import <ConnectionInterface.h>
#import <GenericClientListener.h>

// -------
// Request
// -------

#undef self
#define self Request

class {
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
};

def(void, OnHttpVersion, HTTP_Version version) {
	this->version = version;
}

def(void, OnMethod, HTTP_Method method) {
	this->method = method;
}

def(void, OnPath, String path) {
	String_Copy(&this->path, path);
}

/*
 * These parameters are supplied with the URL, such as:
 * http://localhost:8080/?test=abc&test2=def
 */

def(String *, OnQueryParameter, String name) {
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

def(String *, OnBodyParameter, String name) {
	return call(OnQueryParameter, name);
}

def(void, OnRespond, bool persistent) {
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

		Int64_ToString(this->resp.len));

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

def(void, Init, SocketConnection *conn) {
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
	events.onHeader         = (HTTP_OnHeader) EmptyCallback();
	events.onVersion        = (HTTP_OnVersion) Callback(this, Request_OnHttpVersion);
	events.onMethod         = (HTTP_OnMethod) Callback(this, Request_OnMethod);
	events.onPath           = (HTTP_OnPath) Callback(this, Request_OnPath);
	events.onQueryParameter = (HTTP_OnParameter) Callback(this, Request_OnQueryParameter);
	events.onBodyParameter  = (HTTP_OnParameter) Callback(this, Request_OnBodyParameter);
	events.onRespond        = (HTTP_Server_OnRespond) Callback(this, Request_OnRespond);

	HTTP_Server_Init(&this->server, events, conn, 2048, 4096);
}

def(void, Destroy) {
	HTTP_Server_Destroy(&this->server);

	String_Destroy(&this->resp);
	String_Destroy(&this->path);
	String_Destroy(&this->paramTest);
	String_Destroy(&this->paramTest2);
}

def(Connection_Status, Parse) {
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

def(Connection_Status, Respond) {
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

#undef self
#define self HttpConnection

class {
	Connection base;
	Request request;
};

def(void, Init, SocketConnection *conn) {
	Request_Init(&this->request, conn);
}

def(void, Destroy) {
	Request_Destroy(&this->request);
}

def(Connection_Status, Push) {
	return Request_Parse(&this->request);
}

def(Connection_Status, Pull) {
	return Request_Respond(&this->request);
}

Impl(Connection) = {
	.size    = sizeof(self),
	.init    = (void *) ref(Init),
	.destroy = (void *) ref(Destroy),
	.push    = (void *) ref(Push),
	.pull    = (void *) ref(Pull)
};

// ----
// main
// ----

bool startServer(Server *server, GenericClientListener *listener) {
	try {
		Server_Init(server, 8080, &GenericClientListenerImpl, listener);
		String_Print(String("Server started.\n"));
		excReturn true;
	} clean catch(Socket, excAddressInUse) {
		String_Print(String("The address is already in use!\n"));
		excReturn false;
	} finally {

	} tryEnd;

	return false;
}

int main(void) {
	Signal0();

	GenericClientListener listener;
	GenericClientListener_Init(&listener, &HttpConnectionImpl);

	Server server;

	if (!startServer(&server, &listener)) {
		return ExitStatus_Failure;
	}

	try {
		while (true) {
			Server_Process(&server);
		}
	} clean catch(Signal, excSigInt) {
		String_Print(String("Server shutdown.\n"));
	} catchAny {
		Exception_Print(e);

#if Exception_SaveTrace
		Backtrace_PrintTrace(__exc_mgr.e.trace, __exc_mgr.e.traceItems);
#endif

		excReturn ExitStatus_Failure;
	} finally {
		Server_Destroy(&server);
	} tryEnd;

	return ExitStatus_Success;
}
