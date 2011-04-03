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

#import <Signal.h>
#import <Server.h>
#import <Socket.h>
#import <Integer.h>
#import <HTTP/Method.h>
#import <HTTP/Server.h>
#import <Connection.h>
#import <SocketClient.h>
#import <SocketConnection.h>
#import <DoublyLinkedList.h>
#import <GenericClientListener.h>

// -------
// Request
// -------

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

def(void, OnPath, RdString path) {
	String_Copy(&this->path, path);
}

/*
 * These parameters are supplied with the URL, such as:
 * http://localhost:8080/?test=abc&test2=def
 */

def(String *, OnQueryParameter, RdString name) {
	if (String_Equals(name, $("test"))) {
		/* test's value will be put into this variable. */
		return &this->paramTest;
	} else if (String_Equals(name, $("test2"))) {
		return &this->paramTest2;
	}

	/* Ignore all other parameters */
	return NULL;
}

/*
 * Treat body-submitted parameters equally like URL-supplied ones.
 */

def(String *, OnBodyParameter, RdString name) {
	return call(OnQueryParameter, name);
}

def(void, OnRespond, bool persistent) {
	this->resp.len = 0;

	if (this->method == HTTP_Method_Get) {
		String_Append(&this->resp, $("Page requested via GET."));
	} else if (this->method == HTTP_Method_Post) {
		String_Append(&this->resp, $("Page requested via POST."));
	} else if (this->method == HTTP_Method_Head) {
		String_Append(&this->resp, $("Page requested via HEAD."));
	}

	if (this->method != HTTP_Method_Head) {
		String_Append(&this->resp,
			FmtString($(
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

			this->path.rd,
			this->paramTest.rd,
			this->paramTest2.rd));
	}

	String strLength = Integer_ToString(this->resp.len);

	String envelope = String_Format(
		$(
			"% 200 OK\r\n"
			"Content-Type: text/html; charset=utf-8\r\n"
			"%\r\n"
			"Content-Length: %\r\n"
			"\r\n"
		),

		HTTP_Version_ToString(this->version),

		persistent
			? $("Connection: Keep-Alive")
			: $("Connection: Close"),

		strLength.rd);

	String_Destroy(&strLength);

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
	this->resp       = String_New(2048);
	this->method     = HTTP_Method_Get;
	this->version    = HTTP_Version_1_0;
	this->gotData    = false;
	this->persistent = false;
	this->path       = String_New(0);

	/* paramTest and paramTest2 must not be longer than 256 bytes,
	 * otherwise an `HTTP_Query_ExceedsPermittedLengthException'
	 * exception will be thrown.
	 */

	this->paramTest  = String_New(256);
	this->paramTest2 = String_New(256);

	this->server = HTTP_Server_New(conn, 2048, 4096);

	HTTP_Server_BindVersion(&this->server,
		HTTP_OnVersion_For(this, Request_OnHttpVersion));

	HTTP_Server_BindMethod(&this->server,
		HTTP_OnMethod_For(this, Request_OnMethod));

	HTTP_Server_BindPath(&this->server,
		HTTP_OnPath_For(this, Request_OnPath));

	HTTP_Server_BindQueryParameter(&this->server,
		HTTP_OnParameter_For(this, Request_OnQueryParameter));

	HTTP_Server_BindBodyParameter(&this->server,
		HTTP_OnParameter_For(this, Request_OnBodyParameter));

	HTTP_Server_BindRespond(&this->server,
		HTTP_Server_OnRespond_For(this, Request_OnRespond));
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

#undef self

// --------------
// HttpConnection
// --------------

#define self HttpConnection

class {
	Request request;
};

def(void, Init, SocketConnection *conn, __unused Logger *logger) {
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
	.init    = ref(Init),
	.destroy = ref(Destroy),
	.push    = ref(Push),
	.pull    = ref(Pull)
};

ExportImpl(Connection);

#undef self

// ----
// main
// ----

bool startServer(Server *server, ClientListener listener) {
	try {
		Server_Init(server, 8080, listener);
		String_Print($("Server started.\n"));
		excReturn true;
	} catch(Socket, AddressInUse) {
		String_Print($("The address is already in use!\n"));
		excReturn false;
	} finally {

	} tryEnd;

	return false;
}

int main(void) {
	Signal0();

	Logger logger = Logger_New(Logger_Printer_Empty());

	GenericClientListener listener;
	GenericClientListener_Init(&listener, HttpConnection_GetImpl(), &logger);

	Server server;

	if (!startServer(&server,
		GenericClientListener_AsClientListener(&listener)))
	{
		return ExitStatus_Failure;
	}

	try {
		while (true) {
			Server_Process(&server);
		}
	} catch(Signal, SigInt) {
		String_Print($("Server shutdown.\n"));
	} catchAny {
		Exception_Print(e);

#if Exception_SaveTrace
		Backtrace_PrintTrace(
			Exception_GetTraceBuffer(),
			Exception_GetTraceLength());
#endif

		excReturn ExitStatus_Failure;
	} finally {
		Server_Destroy(&server);
	} tryEnd;

	return ExitStatus_Success;
}
