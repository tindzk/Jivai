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

#import <Client.h>
#import <Signal.h>
#import <Server.h>
#import <Socket.h>
#import <Integer.h>
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

#undef self
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

def(void, OnAccept, Client *client) {
	ClientData *data = New(ClientData);
	data->id = this->activeConn;

	client->data = data;

	String tmp;

	String_FmtPrint(
		String("Got TCP connection from %:%, fd=%\n"),
		tmp = NetworkAddress_ToString(client->conn->addr),
		Integer_ToString(client->conn->addr.port),
		Integer_ToString(client->conn->fd));

	String_Destroy(&tmp);

	String resp = String("Hi.\n");
	SocketConnection_Write(client->conn, resp.buf, resp.len);

	this->activeConn++;
}

def(void, OnDisconnect, Client *client) {
	String tmp;

	String_FmtPrint(
		String("Client %:%, fd=% disconnected\n"),
		tmp = NetworkAddress_ToString(client->conn->addr),
		Integer_ToString(client->conn->addr.port),
		Integer_ToString(client->conn->fd));

	String_Destroy(&tmp);

	Memory_Free(client->data);

	this->activeConn--;
}

def(Connection_Status, OnData, Client *client) {
	String s = StackString(1024);
	s.len = SocketConnection_Read(client->conn, s.buf, s.size);

	String input = String_Trim(s);

	String_FmtPrint(
		String("Received data from fd=%: '%'\n"),
		Int16_ToString(client->conn->fd), input);

	if (String_BeginsWith(input, String("info"))) {
		ClientData *data = client->data;

		String resp = String_Format(
			String("You have the ID %.\n"),
			Int32_ToString(data->id));

		SocketConnection_Write(client->conn, resp.buf, resp.len);

		String_Destroy(&resp);
	} else if (String_BeginsWith(input, String("active"))) {
		String tmp = String_Format(
			String("% active connection(s).\n"),
			Int32_ToString(this->activeConn - 1));

		SocketConnection_Write(client->conn, tmp.buf, tmp.len);
		String_Destroy(&tmp);
	} else if (String_BeginsWith(input, String("exit"))) {
		String tmp = String("Bye.\n");
		SocketConnection_Write(client->conn, tmp.buf, tmp.len);

		/* Do not use SocketConnection_Close(client->conn); because
		 * it would not free all resources related to the client. It
		 * does not emit an "OnDisconnect" event either.
		 */

		call(OnDisconnect, client);
		Client_Destroy(client);

		return Connection_Status_Close;
	}

	return Connection_Status_Open;
}

Impl(ClientListener) = {
	.onInit             = (void *) ref(OnInit),
	.onDestroy          = (void *) ref(OnDestroy),
	.onClientConnect    = (void *) ref(OnConnect),
	.onClientAccept     = (void *) ref(OnAccept),
	.onClientDisconnect = (void *) ref(OnDisconnect),
	.onPush             = (void *) ref(OnData),
	.onPull             = NULL
};

// ----
// main
// ----

int main(void) {
	Signal0();

	Server server;
	CustomClientListener listener;

	try {
		Server_Init(&server, 1337, &CustomClientListenerImpl, &listener);
		Server_SetEdgeTriggered(&server, false);

		String_Print(String("Server started.\n"));

		while (true) {
			Server_Process(&server);
		}
	} clean catch (Signal, excSigInt) {
		String_Print(String("Server shutdown.\n"));
	} finally {
		Server_Destroy(&server);
	} tryEnd;

	return ExitStatus_Success;
}
