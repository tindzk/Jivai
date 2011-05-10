#import "SocketServer.h"

#define self SocketServer

rsdef(self, New, Socket_Protocol protocol, int options) {
	return (self) {
		.socket = Socket_New(protocol, options)
	};
}

def(void, Destroy) {
	Socket_Destroy(&this->socket);
}

def(void, SetReusable, bool enable) {
	int value = enable;

	Channel *ch = Socket_GetChannel(&this->socket);

	int id = Channel_GetId(ch);

	if (!Kernel_setsockopt(id, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value))) {
		throw(SetSocketOption);
	}
}

def(void, Bind, unsigned short port) {
	struct sockaddr_in addr = {
		.sin_family = PF_INET,
		.sin_addr.s_addr = htonl(INADDR_ANY),
		.sin_port = htons(port)
	};

	Channel *ch = Socket_GetChannel(&this->socket);

	int id = Channel_GetId(ch);

	if (!Kernel_bind(id, addr)) {
		if (errno == EADDRINUSE) {
			throw(AddressInUse);
		} else {
			throw(BindFailed);
		}
	}
}

def(void, Listen, size_t maxconns) {
	Channel *ch = Socket_GetChannel(&this->socket);

	int id = Channel_GetId(ch);

	if (!Kernel_listen(id, maxconns)) {
		throw(ListenFailed);
	}
}

def(SocketConnection, Accept, int options) {
	struct sockaddr_in remote;
	int socklen = sizeof(remote);

	int acceptFlags = 0;

	if (!BitMask_Has(options, Socket_Option_Blocking)) {
		BitMask_Set(acceptFlags, SOCK_NONBLOCK);
	}

	if (BitMask_Has(options, Socket_Option_CloseOnExec)) {
		BitMask_Set(acceptFlags, SOCK_CLOEXEC);
	}

	Channel *socketCh = Socket_GetChannel(&this->socket);

	int socketId = Channel_GetId(socketCh);

	int id = Kernel_accept4(socketId, &remote, &socklen, acceptFlags);

	if (id == -1) {
		throw(AcceptFailed);
	}

	int flags = FileStatus_ReadWrite;

	if (!BitMask_Has(options, Socket_Option_Blocking)) {
		BitMask_Set(flags, FileStatus_NonBlock);
	}

	Channel ch = Channel_New(id, flags);

	NetworkAddress addr = {
		.ip   = remote.sin_addr.s_addr,
		.port = ntohs(remote.sin_port)
	};

	return SocketConnection_New(&ch, addr);
}
