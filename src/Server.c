#import "Server.h"
#import "App.h"

static ExceptionManager *exc;

void Server0(ExceptionManager *e) {
	exc = e;
}

def(void, Init, ref(Events) events, bool edgeTriggered, unsigned short port) {
	Poll_Init(&this->poll, (void *) ref(OnEvent), this);
	this->events = events;

	this->edgeTriggered = edgeTriggered;

	Socket_Init(&this->socket, Socket_Protocol_TCP);

	Socket_SetCloexecFlag    (&this->socket, true);
	Socket_SetReusableFlag   (&this->socket, true);
	Socket_SetNonBlockingFlag(&this->socket, true);

	Socket_Listen(&this->socket, port, ref(ConnectionLimit));

	/* Add the server socket to epoll. */
	Poll_AddEvent(&this->poll, NULL, this->socket.fd,
		EPOLLIN  |
		EPOLLHUP |
		EPOLLERR);

	this->events.onInit(this->events.context);
}

def(void, Destroy) {
	Socket_Destroy(&this->socket);
	Poll_Destroy(&this->poll);

	this->events.onDestroy(this->events.context);
}

def(void, Process) {
	Poll_Process(&this->poll, -1);
}

def(void, DestroyClient, Client *client) {
	this->events.onClientDisconnect(this->events.context, client);
	Client_Destroy(client);
}

def(void, AcceptClient) {
	Client *client = Client_New();

	Client_Accept(client, &this->socket);

	this->events.onClientAccept(this->events.context, client);

	int flags =
		EPOLLERR |
		EPOLLHUP |
		EPOLLRDHUP;

	if (this->edgeTriggered) {
		BitMask_Set(flags, EPOLLET);
	}

	if (this->events.onPush != NULL) {
		BitMask_Set(flags, EPOLLIN);
	}

	if (this->events.onPull != NULL) {
		BitMask_Set(flags, EPOLLOUT);
	}

	Poll_AddEvent(&this->poll, client, client->conn->fd, flags);
}

def(void, OnEvent, int events, Client *client) {
	if (client == NULL && BitMask_Has(events, EPOLLIN)) {
		/* Incoming connection. */

		if (this->events.onClientConnect(this->events.context)) {
			call(AcceptClient);
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
		if (!this->events.onPush(this->events.context, client)) {
			client = NULL;
		}
	}

	if (client != NULL && BitMask_Has(events, EPOLLOUT)) {
		/* Client requests data. */
		if (!this->events.onPull(this->events.context, client)) {
			client = NULL;
		}
	}

	if (client != NULL && BitMask_Has(events, EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
		/* Error occured or connection hung up. */
		call(DestroyClient, client);
	}
}
