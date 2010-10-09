#import "Socket.h"

size_t Modules_Socket;

static ExceptionManager *exc;

void Socket0(ExceptionManager *e) {
	Modules_Socket = Module_Register(String("Socket"));

	exc = e;
}

void Socket_Init(Socket *this, Socket_Protocol protocol) {
	this->fd       = -1;
	this->unused   = true;
	this->protocol = protocol;

	int style = (protocol == Socket_Protocol_TCP)
		? SOCK_STREAM
		: SOCK_DGRAM;

	int proto = (protocol == Socket_Protocol_TCP)
		? IPPROTO_TCP
		: IPPROTO_UDP;

	if ((this->fd = Kernel_socket(PF_INET, style, proto)) == -1) {
		throw(exc, excSocketFailed);
	}
}

void Socket_SetNonBlockingFlag(Socket *this, bool enable) {
	int flags = Kernel_fcntl(this->fd, FcntlMode_GetStatus, 0);

	if (flags == -1) {
		throw(exc, excFcntlFailed);
	}

	if (enable) {
		flags |= FileStatus_NonBlock;
	} else {
		flags &= ~FileStatus_NonBlock;
	}

	if (Kernel_fcntl(this->fd, FcntlMode_SetStatus, flags) == -1) {
		throw(exc, excFcntlFailed);
	}
}

void Socket_SetCloexecFlag(Socket *this, bool enable) {
	int flags = Kernel_fcntl(this->fd, FcntlMode_GetDescriptorFlags, 0);

	if (flags == -1) {
		throw(exc, excFcntlFailed);
	}

	if (enable) {
		flags |= FileDescriptorFlags_CloseOnExec;
	} else {
		flags &= ~FileDescriptorFlags_CloseOnExec;
	}

	if (Kernel_fcntl(this->fd, FcntlMode_SetDescriptorFlags, flags) == -1) {
		throw(exc, excFcntlFailed);
	}
}

void Socket_SetReusableFlag(Socket *this, bool enable) {
	int val = enable;

	if (!Kernel_setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val))) {
		throw(exc, excSetSocketOption);
	}
}

void Socket_Listen(Socket *this, unsigned short port, int maxconns) {
	struct sockaddr_in addr;

	addr.sin_family = PF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	errno = 0;

	if (!Kernel_bind(this->fd, addr)) {
		if (errno == EADDRINUSE) {
			throw(exc, excAddressInUse);
		} else {
			throw(exc, excBindFailed);
		}
	}

	if (!Kernel_listen(this->fd, maxconns)) {
		throw(exc, excListenFailed);
	}
}

void Socket_SetLinger(Socket *this) {
	struct linger ling;

	ling.l_onoff  = 1;
	ling.l_linger = 30;

	if (!Kernel_setsockopt(this->fd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling))) {
		throw(exc, excSetSocketOption);
	}
}

SocketConnection Socket_Connect(Socket *this, String hostname, unsigned short port) {
	struct sockaddr_in addr;

	addr.sin_family = PF_INET;
	addr.sin_addr   = NetworkAddress_ResolveHost(hostname);
	addr.sin_port   = htons(port);

	if (!this->unused) {
		Socket_Destroy(this);
		Socket_Init(this, this->protocol);
	}

	if (!Kernel_connect(this->fd, &addr, sizeof(addr))) {
		throw(exc, excConnectFailed);
	}

	this->unused = false;

	SocketConnection conn;

	conn.fd = this->fd;

	conn.addr.ip   = addr.sin_addr.s_addr;
	conn.addr.port = port;

	conn.corking     = false;
	conn.closable    = false;
	conn.nonblocking = false;

	return conn;
}

SocketConnection Socket_Accept(Socket *this) {
	struct sockaddr_in remote;
	socklen_t socklen = sizeof(remote);

	SocketConnection conn;

	if ((conn.fd = Kernel_accept(this->fd, &remote, &socklen)) == -1) {
		throw(exc, excAcceptFailed);
	}

	conn.addr.ip   = remote.sin_addr.s_addr;
	conn.addr.port = ntohs(remote.sin_port);

	conn.corking     = false;
	conn.closable    = true;
	conn.nonblocking = false;

	return conn;
}

void Socket_Destroy(Socket *this) {
	Kernel_shutdown(this->fd, SHUT_RDWR);
	Kernel_close(this->fd);
}
