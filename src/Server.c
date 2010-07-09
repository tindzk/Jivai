#include "Server.h"

static ExceptionManager *exc;

void Server0(ExceptionManager *e) {
	exc = e;
}

void Server_Init(Server *this, Server_Events events, bool edgeTriggered, int port) {
	Poll_Init(&this->poll, SERVER_MAX_CONN, (void *) &Server_OnEvent, this);
	this->events = events;

	this->edgeTriggered = edgeTriggered;

	Socket_Init(&this->socket, Socket_Protocol_TCP);
	Socket_SetReusableFlag(&this->socket);
	Socket_SetCloexecFlag(&this->socket, true);
	Socket_SetNonBlockingFlag(&this->socket, true);
	Socket_Listen(&this->socket, port, SERVER_MAX_CONN);

	/* Add the server socket to epoll. */
	Poll_AddEvent(&this->poll, NULL, this->socket.fd, EPOLLIN | EPOLLHUP | EPOLLERR);

	this->events.onInit(this->events.context);
}

void Server_Destroy(Server *this) {
	Socket_Destroy(&this->socket);
	Poll_Destroy(&this->poll);

	this->events.onDestroy(this->events.context);
}

void Server_Process(Server *this) {
	Poll_Process(&this->poll, -1);
}

void Server_DestroyClient(Server *this, Client *client) {
	this->events.onClientDisconnect(this->events.context, client);
	Client_Destroy(client);
}

void Server_AcceptClient(Server *this) {
	Client *client = Client_New();

	Client_Accept(client, &this->socket);

	this->events.onClientAccept(this->events.context, client);

	int flags = EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLERR;

	if (this->edgeTriggered) {
		BitMask_Set(flags, EPOLLET);
	}

	Poll_AddEvent(&this->poll, client, client->conn->fd, flags);
}

void Server_OnEvent(Server *this, int events, Client *client) {
	if (client == NULL && BitMask_Has(events, EPOLLIN)) {
		/* Incoming connection. */

		if (this->events.onClientConnect(this->events.context)) {
			Server_AcceptClient(this);
		} else {
			/* This is necessary, else the same event will occur
			 * again the next time epoll_wait() gets called.
			 */

			SocketConnection conn = Socket_Accept(&this->socket);
			SocketConnection_Close(&conn);
		}
	}

	if (client != NULL && BitMask_Has(events, EPOLLIN)) {
		/* Receiving data from client. */
		bool close = false;

		try (exc) {
			this->events.onClientData(this->events.context, client);
		} catch(&SocketConnection_NotConnectedException, e) {
			close = true;
		} catch(&SocketConnection_ConnectionResetException, e) {
			close = true;
		} catch(&SocketConnection_LengthMismatchException, e) {
			close = true;
		} finally {
			if (close) {
				Server_DestroyClient(this, client);
				client = NULL;
			}
		} tryEnd;
	}

	if (client != NULL && BitMask_Has(events, EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
		/* Error occured or connection hung up. */
		Server_DestroyClient(this, client);
	}
}
