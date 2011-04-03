#import "Server.h"

#define self Server

def(void, Init, unsigned short port, ClientListener listener) {
	this->listener = listener;
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

	delegate(this->listener, onInit);
}

def(void, Destroy) {
	Socket_Destroy(&this->socket);
	Poll_Destroy(&this->poll);

	delegate(this->listener, onDestroy);
}

def(void, SetEdgeTriggered, bool value) {
	this->edgeTriggered = value;
}

def(void, Process) {
	Poll_Process(&this->poll, -1);
}

def(void, DestroyClient, SocketClient *client) {
	delegate(this->listener, onDisconnect, client);

	SocketClient_Destroy(client);
}

def(void, AcceptClient) {
	SocketClient *client = SocketClient_New();

	SocketClient_Accept(client, &this->socket);

	delegate(this->listener, onAccept, client);

	int flags =
		Poll_Events_Error  |
		Poll_Events_HangUp |
		Poll_Events_PeerHangUp;

	if (this->edgeTriggered) {
		BitMask_Set(flags, Poll_Events_EdgeTriggered);
	}

	if (implements(this->listener, onPush)) {
		BitMask_Set(flags, Poll_Events_Input);
	}

	if (implements(this->listener, onPull)) {
		BitMask_Set(flags, Poll_Events_Output);
	}

	Poll_AddEvent(&this->poll,
		SocketClient_ToGeneric(client),
		SocketClient_GetFd(client),
		flags);
}

def(void, OnEvent, int events, SocketClientExtendedInstance instClient) {
	SocketClient *client = instClient.object;

	if (client == NULL && BitMask_Has(events, Poll_Events_Input)) {
		/* Incoming connection. */

		if (delegate(this->listener, onConnect)) {
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
		if (!delegate(this->listener, onPush, client)) {
			client = NULL;
		}
	}

	if (client != NULL && BitMask_Has(events, Poll_Events_Output)) {
		/* Client requests data. */
		if (!delegate(this->listener, onPull, client)) {
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
