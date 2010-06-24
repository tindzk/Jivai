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

#include <Client.h>
#include <Signal.h>
#include <Server.h>
#include <Socket.h>
#include <Connection.h>
#include <HTTP/Method.h>
#include <HTTP/Server.h>
#include <ClientListener.h>
#include <SocketConnection.h>
#include <ExceptionManager.h>
#include <DoublyLinkedList.h>
#include <ConnectionInterface.h>

ExceptionManager exc;

// -------
// Request
// -------

typedef struct {
	HTTP_Server server;
	HTTP_Method method;
	HTTP_Version version;

	SocketConnection *conn;

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
	if (String_Equals(&name, String("test"))) {
		/* test's value will be put into this variable. */
		return &this->paramTest;
	} else if (String_Equals(&name, String("test2"))) {
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

bool Request_OnRespond(Request *this, bool persistent) {
	String msg = HeapString(2048);

	if (this->method == HTTP_Method_Get) {
		String_Append(&msg, String("Page requested via GET."));
	} else if (this->method == HTTP_Method_Post) {
		String_Append(&msg, String("Page requested via POST."));
	} else if (this->method == HTTP_Method_Head) {
		String_Append(&msg, String("Page requested via HEAD."));
	}

	if (this->method != HTTP_Method_Head) {
		String tmp;
		String_Append(&msg, tmp = String_Format(
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

		Integer_ToString(msg.len)
	);

	SocketConnection_Write(this->conn, envelope.buf, envelope.len);
	SocketConnection_Write(this->conn, msg.buf, msg.len);
	SocketConnection_Flush(this->conn);

	String_Destroy(&envelope);
	String_Destroy(&msg);

	this->paramTest.len  = 0;
	this->paramTest2.len = 0;

	return persistent;
}

void Request_Init(Request *this, SocketConnection *conn) {
	this->conn = conn;

	this->method  = HTTP_Method_Get;
	this->version = HTTP_Version_1_0;

	this->path = HeapString(0);

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

	String_Destroy(&this->path);
	String_Destroy(&this->paramTest);
	String_Destroy(&this->paramTest2);
}

bool Request_Parse(Request *this) {
	bool keepOpen = false;

	keepOpen = HTTP_Server_Process(&this->server);

	return keepOpen;
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

bool HttpConnection_Process(HttpConnection *this) {
	return !Request_Parse(&this->request);
}

ConnectionInterface HttpConnection_Methods = {
	.new     = (void *) HttpConnection_New,
	.init    = (void *) HttpConnection_Init,
	.destroy = (void *) HttpConnection_Destroy,
	.process = (void *) HttpConnection_Process
};

// ----
// main
// ----

int main(void) {
	ExceptionManager_Init(&exc);

	String0(&exc);
	Socket0(&exc);
	Memory0(&exc);
	Signal0(&exc);
	Server0(&exc);
	HTTP_Query0(&exc);
	HTTP_Header0(&exc);
	HTTP_Server0(&exc);
	SocketConnection0(&exc);

	Server server;
	Server_Events events;
	ClientListener listener;
	ClientListener_Init(&listener, &HttpConnection_Methods, &events);

	int res = EXIT_SUCCESS;

	try(&exc) {
		Server_Init(&server, events, 8080);
		String_Print(String("Server started.\n"));

		while (true) {
			Server_Process(&server);
		}
	} catch(&Signal_SigIntException, e) {
		String_Print(String("Server shutdown.\n"));
	} catchAny(e) {
		Exception_Print(e);

#ifdef Exception_SaveTrace
		Backtrace_PrintTrace(e->trace, e->traceItems);
#endif

		res = EXIT_FAILURE;
	} finally {
		Server_Destroy(&server);
	} tryEnd;

	return res;
}
