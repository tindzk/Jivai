#import "Socket.h"

#define self Socket

rsdef(self, New, ref(Protocol) protocol) {
	int style = (protocol == ref(Protocol_TCP))
		? SOCK_STREAM
		: SOCK_DGRAM;

	int proto = (protocol == ref(Protocol_TCP))
		? IPPROTO_TCP
		: IPPROTO_UDP;

	int id = Kernel_socket(PF_INET, style, proto);

	if (id == -1) {
		throw(SocketFailed);
	}

	return (self) {
		.ch       = Channel_New(id, 0),
		.unused   = true,
		.protocol = protocol
	};
}

def(void, Destroy) {
	Kernel_shutdown(Channel_GetId(&this->ch), SHUT_RDWR);
	Channel_Destroy(&this->ch);
}

def(void, SetReusable, bool enable) {
	int value = enable;

	if (!Kernel_setsockopt(Channel_GetId(&this->ch), SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value))) {
		throw(SetSocketOption);
	}
}

def(void, SetLinger) {
	struct linger ling = {
		.l_onoff  = 1,
		.l_linger = 30
	};

	if (!Kernel_setsockopt(Channel_GetId(&this->ch), SOL_SOCKET, SO_LINGER, &ling, sizeof(ling))) {
		throw(SetSocketOption);
	}
}

def(void, Listen, unsigned short port, int maxconns) {
	struct sockaddr_in addr = {
		.sin_family = PF_INET,
		.sin_addr.s_addr = htonl(INADDR_ANY),
		.sin_port = htons(port)
	};

	errno = 0;

	if (!Kernel_bind(Channel_GetId(&this->ch), addr)) {
		if (errno == EADDRINUSE) {
			throw(AddressInUse);
		} else {
			throw(BindFailed);
		}
	}

	if (!Kernel_listen(Channel_GetId(&this->ch), maxconns)) {
		throw(ListenFailed);
	}
}

def(SocketConnection, Connect, RdString hostname, unsigned short port) {
	struct sockaddr_in addr = {
		.sin_family = PF_INET,
		.sin_addr   = NetworkAddress_ResolveHost(hostname),
		.sin_port   = htons(port)
	};

	if (!this->unused) {
		call(Destroy);
		*this = scall(New, this->protocol);
	}

	if (!Kernel_connect(Channel_GetId(&this->ch), &addr, sizeof(addr))) {
		throw(ConnectFailed);
	}

	this->unused = false;

	NetworkAddress naddr = {
		.ip   = addr.sin_addr.s_addr,
		.port = port
	};

	return SocketConnection_New(this->ch, naddr, false);
}

def(SocketConnection, Accept) {
	struct sockaddr_in remote;
	int socklen = sizeof(remote);

	int id = Kernel_accept(Channel_GetId(&this->ch), &remote, &socklen);

	if (id == -1) {
		throw(AcceptFailed);
	}

	Channel ch = Channel_New(id, FileStatus_ReadWrite);

	NetworkAddress addr = {
		.ip   = remote.sin_addr.s_addr,
		.port = ntohs(remote.sin_port)
	};

	return SocketConnection_New(ch, addr, true);
}
