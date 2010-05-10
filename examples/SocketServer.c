/*
 * Usage:
 * ------
 *
 * Start several telnet clients:
 * $ telnet localhost 1337
 *
 * Then type "info", "active" or "exit".
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

// --------------
// ClientListener
// --------------

typedef struct {
	int activeConn;
} ClientListener;

void ClientListener_OnInit(ClientListener *this) {
	this->activeConn = 0;
}

void ClientListener_OnDestroy(UNUSED ClientListener *this) {
	/* ... */
}

bool ClientListener_OnConnect(ClientListener *this) {
	/* Don't allow more than five parallel connections. */
	return this->activeConn < 5;
}

void ClientListener_OnAccept(ClientListener *this, Client *client) {
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

void ClientListener_OnDisconnect(ClientListener *this, Client *client) {
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

void ClientListener_OnData(ClientListener *this, Client *client) {
	String s = StackString(1024);
	s.len = SocketConnection_Read(client->conn, s.buf, s.size);

	String_Trim(&s);

	String tmp;
	String_Print(tmp = String_Format(
		String("Received data from fd=%: '%'\n"),
		Integer_ToString(client->conn->fd), s));
	String_Destroy(&tmp);

	if (String_BeginsWith(&s, String("info"))) {
		ClientData *data = client->data;

		String resp = String_Format(
			String("You have the ID %.\n"),
			Integer_ToString(data->id));

		SocketConnection_Write(client->conn, resp.buf, resp.len);

		String_Destroy(&resp);
	} else if (String_BeginsWith(&s, String("active"))) {
		String tmp = String_Format(
			String("% active connection(s).\n"),
			Integer_ToString(this->activeConn - 1));

		SocketConnection_Write(client->conn, tmp.buf, tmp.len);
		String_Destroy(&tmp);
	} else if (String_BeginsWith(&s, String("exit"))) {
		String tmp = String("Bye.\n");
		SocketConnection_Write(client->conn, tmp.buf, tmp.len);

		/* Do not use SocketConnection_Close(client->conn); because
		 * it would not free all resources related to the client. It
		 * does not emit an "OnDisconnect" event either.
		 */

		ClientListener_OnDisconnect(this, client);
		Client_Destroy(client);
	}
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
	ServerEvents events;
	ClientListener listener;

	events.context = &listener;

	events.onInit             = (void *) &ClientListener_OnInit;
	events.onDestroy          = (void *) &ClientListener_OnDestroy;
	events.onClientConnect    = (void *) &ClientListener_OnConnect; events.onClientAccept     = (void *) &ClientListener_OnAccept;
	events.onClientData       = (void *) &ClientListener_OnData;
	events.onClientDisconnect = (void *) &ClientListener_OnDisconnect;

	try(&exc) {
		Server_Init(&server, &events, 1337);
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
