#import "Socket.h"

Exception_Define(AcceptFailedException);
Exception_Define(AddressInUseException);
Exception_Define(BindFailedException);
Exception_Define(ConnectFailedException);
Exception_Define(FcntlFailedException);
Exception_Define(ListenFailedException);
Exception_Define(SetSocketOptionException);
Exception_Define(SocketFailedException);

static ExceptionManager *exc;

void Socket0(ExceptionManager *e) {
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
		throw(exc, &SocketFailedException);
	}
}

void Socket_SetNonBlockingFlag(Socket *this, bool enable) {
	int flags = syscall(__NR_fcntl, this->fd, FcntlMode_GetStatus, 0);

	if (flags < 0) {
		throw(exc, &FcntlFailedException);
	}

	if (enable) {
		flags |= FileStatus_NonBlock;
	} else {
		flags &= ~FileStatus_NonBlock;
	}

	if (syscall(__NR_fcntl, this->fd, FcntlMode_SetStatus, flags) < 0) {
		throw(exc, &FcntlFailedException);
	}
}

void Socket_SetCloexecFlag(Socket *this, bool enable) {
	int flags = syscall(__NR_fcntl, this->fd, FcntlMode_GetDescriptorFlags, 0);

	if (flags < 0) {
		throw(exc, &FcntlFailedException);
	}

	if (enable) {
		flags |= FileDescriptorFlags_CloseOnExec;
	} else {
		flags &= ~FileDescriptorFlags_CloseOnExec;
	}

	if (syscall(__NR_fcntl, this->fd, FcntlMode_SetDescriptorFlags, flags) < 0) {
		throw(exc, &FcntlFailedException);
	}
}

void Socket_SetReusableFlag(Socket *this) {
	int opt = 1;

	long args[] = { this->fd, SOL_SOCKET, SO_REUSEADDR, (long) &opt, sizeof(opt) };

	if (syscall(__NR_socketcall, SYS_SETSOCKOPT, args) < 0) {
		throw(exc, &SetSocketOptionException);
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
			throw(exc, &AddressInUseException);
		} else {
			throw(exc, &BindFailedException);
		}
	}

	long args2[] = { this->fd, maxconns };

	if (syscall(__NR_socketcall, SYS_LISTEN, args2) < 0) {
		throw(exc, &ListenFailedException);
	}
}

void Socket_SetLinger(Socket *this) {
	struct linger ling;

	ling.l_onoff  = 1;
	ling.l_linger = 30;

	long args[] = { this->fd, SOL_SOCKET, SO_LINGER, (long) &ling, sizeof(ling) };

	if (syscall(__NR_socketcall, SYS_SETSOCKOPT, args) < 0) {
		throw(exc, &SetSocketOptionException);
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
		throw(exc, &ConnectFailedException);
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
		throw(exc, &AcceptFailedException);
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
