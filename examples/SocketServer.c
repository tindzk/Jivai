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

#include <Client.h>
#include <Signal.h>
#include <Server.h>
#include <Socket.h>
#include <SocketConnection.h>
#include <ExceptionManager.h>

ExceptionManager exc;

// ----------
// ClientData
// ----------

typedef struct {
	int id;
} ClientData;

// --------------------
// CustomClientListener
// --------------------

typedef struct {
	int activeConn;
} CustomClientListener;

void CustomClientListener_OnInit(CustomClientListener *this) {
	this->activeConn = 0;
}

void CustomClientListener_OnDestroy(__unused CustomClientListener *this) {
	/* ... */
}

bool CustomClientListener_OnConnect(CustomClientListener *this) {
	/* Don't allow more than five parallel connections. */
	return this->activeConn < 5;
}

void CustomClientListener_OnAccept(CustomClientListener *this, Client *client) {
	ClientData *data = New(ClientData);
	data->id = this->activeConn;

	client->data = data;

	String tmp, tmp2;
	String_Print(tmp = String_Format(
		String("Got TCP connection from %:%, fd=%\n"),
		tmp2 = NetworkAddress_ToString(client->conn->addr),
		Integer_ToString(client->conn->addr.port),
		Integer_ToString(client->conn->fd)));

	String_Destroy(&tmp);
	String_Destroy(&tmp2);

	String resp = String("Hi.\n");
	SocketConnection_Write(client->conn, resp.buf, resp.len);

	this->activeConn++;
}

void CustomClientListener_OnDisconnect(CustomClientListener *this, Client *client) {
	String tmp, tmp2;
	String_Print(tmp = String_Format(
		String("Client %:%, fd=% disconnected\n"),
		tmp2 = NetworkAddress_ToString(client->conn->addr),
		Integer_ToString(client->conn->addr.port),
		Integer_ToString(client->conn->fd)));

	String_Destroy(&tmp);
	String_Destroy(&tmp2);

	Memory_Free(client->data);

	this->activeConn--;
}

Connection_Status CustomClientListener_OnData(CustomClientListener *this, Client *client) {
	String s = StackString(1024);
	s.len = SocketConnection_Read(client->conn, s.buf, s.size);

	String_Trim(&s);

	String tmp;
	String_Print(tmp = String_Format(
		String("Received data from fd=%: '%'\n"),
		Integer_ToString(client->conn->fd), s));
	String_Destroy(&tmp);

	if (String_BeginsWith(s, String("info"))) {
		ClientData *data = client->data;

		String resp = String_Format(
			String("You have the ID %.\n"),
			Integer_ToString(data->id));

		SocketConnection_Write(client->conn, resp.buf, resp.len);

		String_Destroy(&resp);
	} else if (String_BeginsWith(s, String("active"))) {
		String tmp = String_Format(
			String("% active connection(s).\n"),
			Integer_ToString(this->activeConn - 1));

		SocketConnection_Write(client->conn, tmp.buf, tmp.len);
		String_Destroy(&tmp);
	} else if (String_BeginsWith(s, String("exit"))) {
		String tmp = String("Bye.\n");
		SocketConnection_Write(client->conn, tmp.buf, tmp.len);

		/* Do not use SocketConnection_Close(client->conn); because
		 * it would not free all resources related to the client. It
		 * does not emit an "OnDisconnect" event either.
		 */

		CustomClientListener_OnDisconnect(this, client);
		Client_Destroy(client);

		return Connection_Status_Close;
	}

	return Connection_Status_Open;
}

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
	SocketConnection0(&exc);

	Server server;
	Server_Events events;
	CustomClientListener listener;

	events.context = &listener;

	events.onInit             = (void *) &CustomClientListener_OnInit;
	events.onDestroy          = (void *) &CustomClientListener_OnDestroy;
	events.onClientConnect    = (void *) &CustomClientListener_OnConnect;
	events.onClientAccept     = (void *) &CustomClientListener_OnAccept;
	events.onClientDisconnect = (void *) &CustomClientListener_OnDisconnect;
	events.onPush             = (void *) &CustomClientListener_OnData;
	events.onPull             = NULL;

	try(&exc) {
		Server_Init(&server, events, false, 1337);
		String_Print(String("Server started.\n"));

		while (true) {
			Server_Process(&server);
		}
	} catch(&Signal_SigIntException, e) {
		String_Print(String("Server shutdown.\n"));
	} finally {
		Server_Destroy(&server);
	} tryEnd;

	return EXIT_SUCCESS;
}
