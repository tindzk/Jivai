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

#import <Main.h>
#import <Server.h>
#import <Socket.h>
#import <Integer.h>
#import <EventLoop.h>
#import <HTTP/Method.h>
#import <HTTP/Server.h>
#import <SocketConnection.h>
#import <DoublyLinkedList.h>

// --------------
// HttpConnection
// --------------

#define self HttpConnection

class {
	bool persistent;

	HTTP_Server  server;
	HTTP_Method  method;
	HTTP_Version version;

	SocketConnection *conn;

	String resp;
	String path;

	String paramTest;
	String paramTest2;

	Logger *logger;

	Server_Client *client;
};

def(void, OnRequest) {
	String strFd = Integer_ToString(this->conn->ch.id);
	Logger_Info(this->logger, $("Receiving request (via fd=%)."), strFd.rd);
	String_Destroy(&strFd);
}

def(void, OnRequestInfo, HTTP_RequestInfo info) {
	Logger_Info(this->logger, $("Client requested '%'."), info.path);

	String_Copy(&this->path, info.path);
	this->method  = info.method;
	this->version = info.version;
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

/* Treat body-submitted parameters equal to URL-supplied ones. */
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

	SocketConnection_Write(this->conn, envelope.rd);

	size_t written = SocketConnection_Write(this->conn, this->resp.rd);
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

def(void, Init, Server_Client *client, Logger *logger) {
	this->logger     = logger;
	this->client     = client;
	this->conn       = &client->socket.conn;
	this->resp       = String_New(2048);
	this->method     = HTTP_Method_Get;
	this->version    = HTTP_Version_1_0;
	this->persistent = false;
	this->path       = String_New(0);

	/* paramTest and paramTest2 must not be longer than 256 bytes,
	 * otherwise an `HTTP_Query_ExceedsPermittedLengthException'
	 * exception will be thrown.
	 */

	this->paramTest  = String_New(256);
	this->paramTest2 = String_New(256);

	this->server = HTTP_Server_New(this->conn, 2048, 4096);

	HTTP_Server_BindRequest(&this->server,
		HTTP_Server_OnRequest_For(this, ref(OnRequest)));

	HTTP_Server_BindRequestInfo(&this->server,
		HTTP_OnRequestInfo_For(this, ref(OnRequestInfo)));

	HTTP_Server_BindQueryParameter(&this->server,
		HTTP_OnParameter_For(this, ref(OnQueryParameter)));

	HTTP_Server_BindBodyParameter(&this->server,
		HTTP_OnParameter_For(this, ref(OnBodyParameter)));

	HTTP_Server_BindRespond(&this->server,
		HTTP_Server_OnRespond_For(this, ref(OnRespond)));

	String strFd = Integer_ToString(this->conn->ch.id);
	Logger_Info(this->logger, $("Incoming connection (fd=%)."), strFd.rd);
	String_Destroy(&strFd);
}

def(void, Destroy) {
	String strFd = Integer_ToString(this->conn->ch.id);
	Logger_Info(this->logger, $("Closing connection (fd=%)."), strFd.rd);
	String_Destroy(&strFd);

	HTTP_Server_Destroy(&this->server);

	String_Destroy(&this->resp);
	String_Destroy(&this->path);
	String_Destroy(&this->paramTest);
	String_Destroy(&this->paramTest2);
}

def(void, Parse) {
	bool incomplete = HTTP_Server_Process(&this->server);

	/* Whilst the request is incomplete, keep the connection alive. */
	if (incomplete) {
		return;
	}

	/* Does the client support persistent connections? */
	if (!this->persistent) {
		Server_Client_Close(this->client);
	}
}

def(void, Respond) {
	if (this->resp.len == 0) {
		return call(Parse);
	}

	size_t written = SocketConnection_Write(this->conn, this->resp.rd);
	String_Crop(&this->resp, written);

	if (this->resp.len > 0) {
		/* Some parts are still unsent. Therefore, the connection must stay
		 * open.
		 */
		return;
	}

	if (!this->persistent) {
		Server_Client_Close(this->client);
	}
}

Impl(Connection) = {
	.size    = sizeof(self),
	.init    = ref(Init),
	.destroy = ref(Destroy),
	.pull    = ref(Parse),
	.push    = ref(Respond)
};

ExportImpl(Connection);

#undef self

// -----------
// Application
// -----------

#define self Application

def(void, shutdown, __unused Signal_Type type) {
	Logger_Info(&this->logger, $("Server shutdown."));
	EventLoop_quit(EventLoop_GetInstance());
}

def(bool, Run) {
	Signal_listen(Signal_GetInstance());

	Signal_uponTermination(Signal_GetInstance(),
		Signal_OnTerminate_For(this, ref(shutdown)));

	Server server = Server_new(HttpConnection_GetImpl(), &this->logger);

	try {
		Server_listen(&server, 8080);
		Logger_Info(&this->logger, $("Server started."));
	} catch(SocketServer, AddressInUse) {
		Logger_Error(&this->logger, $("The address is already in use!"));
		excReturn false;
	} finally {

	} tryEnd;

	try {
		EventLoop_run(EventLoop_GetInstance());
	} catchAny {
		Exception_Print(e);

#if Exception_SaveTrace
		Backtrace_PrintTrace(
			Exception_GetTraceBuffer(),
			Exception_GetTraceLength());
#endif

		excReturn false;
	} finally {
		Server_destroy(&server);
	} tryEnd;

	return true;
}
