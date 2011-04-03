#import "Server.h"

#define self Server

def(void, Init, unsigned short port, ConnectionInterface *conn, Logger *logger) {
	this->conn   = conn;
	this->logger = logger;
	this->edgeTriggered = true;

	Poll_Init(&this->poll, Poll_OnEvent_For(this, ref(OnEvent)));

	Socket_Init(&this->socket, Socket_Protocol_TCP);

	Socket_SetCloexecFlag    (&this->socket, true);
	Socket_SetReusableFlag   (&this->socket, true);
	Socket_SetNonBlockingFlag(&this->socket, true);

	Socket_Listen(&this->socket, port, ref(ConnectionLimit));

	/* Add the server socket to epoll. */
	Poll_AddEvent(&this->poll, Generic_Null(), this->socket.fd,
		Poll_Events_Error |
		Poll_Events_Input |
		Poll_Events_HangUp);

	DoublyLinkedList_Init(&this->conns);
}

def(void, Destroy) {
	Socket_Destroy(&this->socket);
	Poll_Destroy(&this->poll);

	/* Shut down all remaining connections. */
	void (^destroy)(SocketClient *) = ^(SocketClient *client) {
		this->conn->destroy(&client->object);
		SocketClient_Destroy(client);
	};

	DoublyLinkedList_Destroy(&this->conns, destroy);
}

def(void, SetEdgeTriggered, bool value) {
	this->edgeTriggered = value;
}

def(void, Process) {
	Poll_Process(&this->poll, -1);
}

def(void, DestroyClient, SocketClient *client) {
	/* Destroy all data associated with the client. */
	this->conn->destroy(&client->object);

	DoublyLinkedList_Remove(&this->conns, client);

	/* Close the connection. */
	SocketClient_Destroy(client);
}

static def(Connection_Status, OnData, SocketClient *client, bool pull) {
	Connection_Status status = Connection_Status_Open;

	try {
		status = pull
			? this->conn->pull(&client->object)
			: this->conn->push(&client->object);
	} catch(SocketConnection, NotConnected) {
		status = Connection_Status_Close;
	} catch(SocketConnection, ConnectionReset) {
		status = Connection_Status_Close;
	} finally {

	} tryEnd;

	if (status == Connection_Status_Close) {
		call(DestroyClient, client);
	}

	return status;
}

static def(void, AcceptClient) {
	SocketClient *client = SocketClient_New(this->conn->size);

	SocketClient_Accept(client, &this->socket);

	this->conn->init(&client->object, client->conn, this->logger);

	/* Add the client to the list of currently active connections. */
	DoublyLinkedList_InsertEnd(&this->conns, client);

	int flags =
		Poll_Events_Error  |
		Poll_Events_HangUp |
		Poll_Events_PeerHangUp;

	if (this->edgeTriggered) {
		BitMask_Set(flags, Poll_Events_EdgeTriggered);
	}

	if (this->conn->push != NULL) {
		BitMask_Set(flags, Poll_Events_Input);
	}

	if (this->conn->pull != NULL) {
		BitMask_Set(flags, Poll_Events_Output);
	}

	Poll_AddEvent(&this->poll,
		SocketClient_ToGeneric(client), client->conn->fd, flags);
}

def(void, OnEvent, int events, SocketClientExtendedInstance instClient) {
	SocketClient *client = instClient.object;

	if (client == NULL && BitMask_Has(events, Poll_Events_Input)) {
		/* Incoming connection. */

		if (true) { /* TODO Make it possible to limit incoming connections. */
			call(AcceptClient);
		} else {
			/* This is necessary, else the same event will occur
			 * again the next time epoll_wait() gets called.
			 */

			SocketConnection conn = Socket_Accept(&this->socket);
			SocketConnection_Close(&conn);
		}
	}

	if (client != NULL && BitMask_Has(events, Poll_Events_Input)) {
		/* Receiving data from client. */
		if (!call(OnData, client, false)) {
			client = NULL;
		}
	}

	if (client != NULL && BitMask_Has(events, Poll_Events_Output)) {
		/* Client requests data. */
		if (!call(OnData, client, true)) {
			client = NULL;
		}
	}

	if (client != NULL &&
		BitMask_Has(events,
			Poll_Events_Error  |
			Poll_Events_HangUp |
			Poll_Events_PeerHangUp))
	{
		/* Error occured or connection hung up. */
		call(DestroyClient, client);
	}
}
