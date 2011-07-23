#import "SocketClient.h"

#define self SocketClient

rsdef(self, New, Socket_Protocol protocol, int options) {
	return (self) {
		.protocol = protocol,
		.options  = options
	};
}

def(void, Destroy) { }

def(SocketConnection, Connect, RdString hostname, unsigned short port) {
	struct sockaddr_in addr = {
		.sin_family = PF_INET,
		.sin_addr   = NetworkAddress_ResolveHost(hostname),
		.sin_port   = htons(port)
	};

	Socket socket = Socket_New(this->protocol, this->options);

	Channel *ch = Socket_GetChannel(&socket);

	int id = Channel_GetId(ch);

	if (!Kernel_connect(id, &addr, sizeof(addr))) {
		errno == EINPROGRESS || throw(ConnectFailed);
	}

	NetworkAddress naddr = {
		.ip   = addr.sin_addr.s_addr,
		.port = port
	};

	return SocketConnection_New(ch, naddr);
}
