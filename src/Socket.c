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

	long args[] = {
		PF_INET,

		(protocol == Socket_Protocol_TCP)
			? SOCK_STREAM
			: SOCK_DGRAM,

		(protocol == Socket_Protocol_TCP)
			? IPPROTO_TCP
			: IPPROTO_UDP
	};

	if ((this->fd = syscall(__NR_socketcall, SYS_SOCKET, args)) < 0) {
		throw(exc, excSocketFailed);
	}
}

void Socket_SetNonBlockingFlag(Socket *this, bool enable) {
	int flags = syscall(__NR_fcntl, this->fd, FcntlMode_GetStatus, 0);

	if (flags < 0) {
		throw(exc, excFcntlFailed);
	}

	if (enable) {
		flags |= FileStatus_NonBlock;
	} else {
		flags &= ~FileStatus_NonBlock;
	}

	if (syscall(__NR_fcntl, this->fd, FcntlMode_SetStatus, flags) < 0) {
		throw(exc, excFcntlFailed);
	}
}

void Socket_SetCloexecFlag(Socket *this, bool enable) {
	int flags = syscall(__NR_fcntl, this->fd, FcntlMode_GetDescriptorFlags, 0);

	if (flags < 0) {
		throw(exc, excFcntlFailed);
	}

	if (enable) {
		flags |= FileDescriptorFlags_CloseOnExec;
	} else {
		flags &= ~FileDescriptorFlags_CloseOnExec;
	}

	if (syscall(__NR_fcntl, this->fd, FcntlMode_SetDescriptorFlags, flags) < 0) {
		throw(exc, excFcntlFailed);
	}
}

void Socket_SetReusableFlag(Socket *this, bool enable) {
	int opt = enable;

	long args[] = { this->fd, SOL_SOCKET, SO_REUSEADDR, (long) &opt, sizeof(opt) };

	if (syscall(__NR_socketcall, SYS_SETSOCKOPT, args) < 0) {
		throw(exc, excSetSocketOption);
	}
}

void Socket_Listen(Socket *this, unsigned short port, int maxconns) {
	struct sockaddr_in addr;

	addr.sin_family = PF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	errno = 0;

	long args[] = { this->fd, (long) &addr, sizeof(addr) };

	if (syscall(__NR_socketcall, SYS_BIND, args) < 0) {
		if (errno == EADDRINUSE) {
			throw(exc, excAddressInUse);
		} else {
			throw(exc, excBindFailed);
		}
	}

	long args2[] = { this->fd, maxconns };

	if (syscall(__NR_socketcall, SYS_LISTEN, args2) < 0) {
		throw(exc, excListenFailed);
	}
}

void Socket_SetLinger(Socket *this) {
	struct linger ling;

	ling.l_onoff  = 1;
	ling.l_linger = 30;

	long args[] = { this->fd, SOL_SOCKET, SO_LINGER, (long) &ling, sizeof(ling) };

	if (syscall(__NR_socketcall, SYS_SETSOCKOPT, args) < 0) {
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

	long args[] = { this->fd, (long) &addr, sizeof(addr) };

	if (syscall(__NR_socketcall, SYS_CONNECT, args) < 0) {
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

	long args[] = { this->fd, (long) &remote, (long) &socklen };

	SocketConnection conn;

	if ((conn.fd = syscall(__NR_socketcall, SYS_ACCEPT, args)) < 0) {
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
	long args[] = { this->fd, SHUT_RDWR };
	syscall(__NR_socketcall, SYS_SHUTDOWN, args);

	syscall(__NR_close, this->fd);
}
