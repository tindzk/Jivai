#import "Server.h"
#import "App.h"

static ExceptionManager *exc;

void Server0(ExceptionManager *e) {
	exc = e;
}

def(void, Init, unsigned short port, ClientListenerInterface *listener, GenericInstance context) {
	this->context  = context;
	this->listener = listener;

	this->edgeTriggered = true;

	Poll_Init(&this->poll, Callback(this, ref(OnEvent)));

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

	this->listener->onInit(this->context);
}

def(void, Destroy) {
	Socket_Destroy(&this->socket);
	Poll_Destroy(&this->poll);

	this->listener->onDestroy(this->context);
}

def(void, SetEdgeTriggered, bool value) {
	this->edgeTriggered = value;
}

def(void, Process) {
	Poll_Process(&this->poll, -1);
}

def(void, DestroyClient, ClientInstance client) {
	this->listener->onClientDisconnect(this->context, client);

	Client_Destroy(client);
	Client_Free(client);
}

def(void, AcceptClient) {
	ClientInstance client = Client_New();

	Client_Init(client);
	Client_Accept(client, &this->socket);

	this->listener->onClientAccept(this->context, client);

	int flags =
		EPOLLERR |
		EPOLLHUP |
		EPOLLRDHUP;

	if (this->edgeTriggered) {
		BitMask_Set(flags, EPOLLET);
	}

	if (this->listener->onPush != NULL) {
		BitMask_Set(flags, EPOLLIN);
	}

	if (this->listener->onPull != NULL) {
		BitMask_Set(flags, EPOLLOUT);
	}

	Poll_AddEvent(&this->poll,
		Client_ToGeneric(client),
		Client_GetFd(client),
		flags);
}

def(void, OnEvent, int events, ClientInstance client) {
	if (Client_IsNull(client) && BitMask_Has(events, EPOLLIN)) {
		/* Incoming connection. */

		if (this->listener->onClientConnect(this->context)) {
			call(AcceptClient);
		} else {
			/* This is necessary, else the same event will occur
			 * again the next time epoll_wait() gets called.
			 */

			SocketConnection conn = Socket_Accept(&this->socket);
			SocketConnection_Close(&conn);
		}
	}

	if (!Client_IsNull(client) && BitMask_Has(events, EPOLLIN)) {
		/* Receiving data from client. */
		if (!this->listener->onPush(this->context, client)) {
			client = Client_Null();
		}
	}

	if (!Client_IsNull(client) && BitMask_Has(events, EPOLLOUT)) {
		/* Client requests data. */
		if (!this->listener->onPull(this->context, client)) {
			client = Client_Null();
		}
	}

	if (!Client_IsNull(client) && BitMask_Has(events, EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
		/* Error occured or connection hung up. */
		call(DestroyClient, client);
	}
}
