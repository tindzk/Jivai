/*
 * Usage:
 * ------
 *
 * Start several telnet clients:
 * $ telnet localhost 1337
 *
 * Then type "info", "active" or "exit".
 *
 * This example shows the possibilities of using a custom client
 * listener like reacting on incoming connections just as they
 * arrive. However, this low-level approach is hardly ever needed
 * and it's generally advisable to use the default client
 * listener which also has the advantage of keeping track of all
 * active connections and closing them automatically on shutdown.
 * See also the httpd.c which makes use of it.
 */

#import <Signal.h>
#import <Server.h>
#import <Socket.h>
#import <Integer.h>
#import <SocketClient.h>
#import <SocketConnection.h>

// ----------
// ClientData
// ----------

record(ClientData) {
	int id;
};

// --------------------
// CustomClientListener
// --------------------

#define self CustomClientListener

class {
	ssize_t activeConn;
};

def(void, OnInit) {
	this->activeConn = 0;
}

def(void, OnDestroy) {
	/* ... */
}

def(bool, OnConnect) {
	/* Don't allow more than five parallel connections. */
	return this->activeConn < 5;
}

def(void, OnAccept, SocketClient *client) {
	ClientData *data = New(ClientData);
	data->id = this->activeConn;

	client->data = data;

	String tmp;

	String_FmtPrint(
		$("Got TCP connection from %:%, fd=%\n"),
		tmp = NetworkAddress_ToString(client->conn->addr),
		Integer_ToString(client->conn->addr.port),
		Integer_ToString(client->conn->fd));

	String_Destroy(&tmp);

	String resp = $("Hi.\n");
	SocketConnection_Write(client->conn, resp.buf, resp.len);

	this->activeConn++;
}

def(void, OnDisconnect, SocketClient *client) {
	String tmp;

	String_FmtPrint(
		$("Client %:%, fd=% disconnected\n"),
		tmp = NetworkAddress_ToString(client->conn->addr),
		Integer_ToString(client->conn->addr.port),
		Integer_ToString(client->conn->fd));

	String_Destroy(&tmp);

	Memory_Free(client->data);

	this->activeConn--;
}

def(ClientConnection_Status, OnData, SocketClient *client) {
	String s = StackString(1024);
	s.len = SocketConnection_Read(client->conn, s.buf, s.size);

	String input = String_Trim(s);

	String_FmtPrint(
		$("Received data from fd=%: '%'\n"),
		Int16_ToString(client->conn->fd), input);

	if (String_BeginsWith(input, $("info"))) {
		ClientData *data = client->data;

		String resp = String_Format(
			$("You have the ID %.\n"),
			Int32_ToString(data->id));

		SocketConnection_Write(client->conn, resp.buf, resp.len);

		String_Destroy(&resp);
	} else if (String_BeginsWith(input, $("active"))) {
		String tmp = String_Format(
			$("% active connection(s).\n"),
			Int32_ToString(this->activeConn - 1));

		SocketConnection_Write(client->conn, tmp.buf, tmp.len);
		String_Destroy(&tmp);
	} else if (String_BeginsWith(input, $("exit"))) {
		String tmp = $("Bye.\n");
		SocketConnection_Write(client->conn, tmp.buf, tmp.len);

		/* Do not use SocketConnection_Close(client->conn); because
		 * it would not free all resources related to the client. It
		 * does not emit an "OnDisconnect" event either.
		 */

		call(OnDisconnect, client);

		SocketClient_Destroy(client);
		SocketClient_Free(client);

		return ClientConnection_Status_Close;
	}

	return ClientConnection_Status_Open;
}

Impl(ClientListener) = {
	.onInit             = (void *) ref(OnInit),
	.onDestroy          = (void *) ref(OnDestroy),
	.onClientConnect    = (void *) ref(OnConnect),
	.onClientAccept     = (void *) ref(OnAccept),
	.onClientDisconnect = (void *) ref(OnDisconnect),
	.onPush             = (void *) ref(OnData)
};

ExportImpl(ClientListener);

#undef self

// ----
// main
// ----

int main(void) {
	Signal0();

	Server server;
	CustomClientListener listener;

	try {
		Server_Init(&server, 1337,
			CustomClientListener_AsClientListener(&listener));

		Server_SetEdgeTriggered(&server, false);

		String_Print($("Server started.\n"));

		while (true) {
			Server_Process(&server);
		}
	} clean catch (Signal, SigInt) {
		String_Print($("Server shutdown.\n"));
	} finally {
		Server_Destroy(&server);
	} tryEnd;

	return ExitStatus_Success;
}
