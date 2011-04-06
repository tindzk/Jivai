#import "Server.h"

#define self Server

static def(void, OnEvent, int events, GenericInstance inst);

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

	DoublyLinkedList_Init(&this->clients);
}

static def(void, _DestroyClient, GenericInstance inst) {
	ref(Client) *client = inst.object;

	/* Destroy all data associated with the client. */
	this->conn->destroy(&client->object);

	/* Close the connection. */
	SocketConnection_Close(client->client.conn);

	Pool_Free(Pool_GetInstance(), client->client.conn);
	Pool_Free(Pool_GetInstance(), client);
}

def(void, Destroy) {
	Socket_Destroy(&this->socket);
	Poll_Destroy(&this->poll);

	/* Shut down all remaining connections. */
	DoublyLinkedList_Destroy(&this->clients,
		LinkedList_OnDestroy_For(this, ref(_DestroyClient)));
}

def(void, SetEdgeTriggered, bool value) {
	this->edgeTriggered = value;
}

def(void, Process) {
	Poll_Process(&this->poll, -1);
}

def(void, DestroyClient, ref(Client) *client) {
	DoublyLinkedList_Remove(&this->clients, client);
	call(_DestroyClient, client);
}

static def(Connection_Status, OnData, ref(Client) *client, bool pull) {
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

static def(void, SocketAccept, ref(Client) *client) {
	SocketConnection conn = Socket_Accept(&this->socket);

	client->client.conn =
		SocketConnection_GetObject(
			SocketConnection_Clone(&conn));

	client->client.conn->corking     = true;
	client->client.conn->nonblocking = true;
}

static def(void, AcceptClient) {
	ref(Client) *client = Pool_Alloc(Pool_GetInstance(),
		sizeof(ref(Client)) + this->conn->size);

	client->client.state = Connection_State_Established;

	call(SocketAccept, client);

	this->conn->init(&client->object, &client->client, this->logger);

	/* Add the client to the list of currently active connections. */
	DoublyLinkedList_InsertEnd(&this->clients, client);

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

	Poll_AddEvent(&this->poll, client, client->client.conn->fd, flags);
}

static def(void, OnEvent, int events, GenericInstance inst) {
	ref(Client) *client = inst.object;

	if (client == NULL) {
		if (BitMask_Has(events, Poll_Events_Input)) {
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

		return;
	}

	if (BitMask_Has(events,
			Poll_Events_Error  |
			Poll_Events_HangUp |
			Poll_Events_PeerHangUp))
	{
		/* Error occured or connection hung up. */
		call(DestroyClient, client);
		return;
	}

	if (BitMask_Has(events, Poll_Events_Input)) {
		/* Receiving data from client. */
		if (!call(OnData, client, false)) {
			return;
		}
	}

	if (BitMask_Has(events, Poll_Events_Output)) {
		/* Client requests data. */
		call(OnData, client, true);
	}
}
