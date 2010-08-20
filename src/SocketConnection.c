#import "SocketConnection.h"

static ExceptionManager *exc;

Exception_Define(SocketConnection_ConnectionRefusedException);
Exception_Define(SocketConnection_ConnectionResetException);
Exception_Define(SocketConnection_FcntlFailedException);
Exception_Define(SocketConnection_FileDescriptorUnusableException);
Exception_Define(SocketConnection_InvalidFileDescriptorException);
Exception_Define(SocketConnection_NotConnectedException);
Exception_Define(SocketConnection_UnknownErrorException);

void SocketConnection0(ExceptionManager *e) {
	exc = e;
}

void SocketConnection_Flush(SocketConnection *this) {
	if (this->corking) {
		int state = 0;

		long args[] = { this->fd, IPPROTO_TCP, TCP_CORK, (long) &state, sizeof(state) };

		syscall(__NR_socketcall, SYS_SETSOCKOPT, args);
	}
}

ssize_t SocketConnection_Read(SocketConnection *this, void *buf, size_t len) {
	int flags = this->nonblocking ? MSG_DONTWAIT : 0;

	errno = 0;

	long args[] = { this->fd, (long) buf, len, flags };

	int res = syscall(__NR_socketcall, SYS_RECV, args);

	if (res >= 0) {
		return res;
	}

	if (errno == EWOULDBLOCK || errno == EAGAIN) {
		return -1;
	} else if (errno == ECONNRESET) {
		throw(exc, &SocketConnection_ConnectionResetException);
	} else if (errno == ECONNREFUSED) {
		throw(exc, &SocketConnection_ConnectionRefusedException);
	} else if (errno == ENOTCONN) {
		throw(exc, &SocketConnection_NotConnectedException);
	} else if (errno == EBADF) {
		throw(exc, &SocketConnection_InvalidFileDescriptorException);
	} else {
		throw(exc, &SocketConnection_UnknownErrorException);
	}

	return 0;
}

bool SocketConnection_SendFile(SocketConnection *this, File *file, off64_t *offset, size_t len) {
	if (this->nonblocking) {
		if (syscall(__NR_fcntl, this->fd,
			FcntlMode_SetStatus,
			FileStatus_ReadWrite | FileStatus_NonBlock) == -1)
		{
			throw(exc, &SocketConnection_FcntlFailedException);
		}
	}

	while (len > 0) {
		size_t write = (len > SocketConnection_ChunkSize)
			? SocketConnection_ChunkSize
			: len;

		ssize_t res;

		do {
			errno = 0;
			res = syscall(__NR_sendfile64, this->fd, file->fd, offset, write);
		} while (res == -1 && errno == EINTR);

		if (res == -1) {
			if (errno == EAGAIN) {
				return false;
			} else if (errno == EBADF) {
				throw(exc, &SocketConnection_InvalidFileDescriptorException);
			} else if (errno == EINVAL) {
				throw(exc, &SocketConnection_FileDescriptorUnusableException);
			} else {
				throw(exc, &SocketConnection_UnknownErrorException);
			}
		} else if (res == 0) {
			break;
		}

		len -= res;
	}

	if (this->nonblocking) {
		if (syscall(__NR_fcntl, this->fd, FcntlMode_SetStatus, FileStatus_ReadWrite) == -1) {
			throw(exc, &SocketConnection_FcntlFailedException);
		}
	}

	return true;
}

ssize_t SocketConnection_Write(SocketConnection *this, void *buf, size_t len) {
	int flags = MSG_NOSIGNAL;

	if (this->corking) {
		flags |= MSG_MORE;
	}

	if (this->nonblocking) {
		flags |= MSG_DONTWAIT;
	}

	errno = 0;


	long args[] = { this->fd, (long) buf, len, flags };

	ssize_t res = syscall(__NR_socketcall, SYS_SEND, args);

	if (res == -1) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			return -1;
		} else if (errno == ECONNRESET) {
			throw(exc, &SocketConnection_ConnectionResetException);
		} else if (errno == EPIPE || errno == ENOTCONN) {
			throw(exc, &SocketConnection_NotConnectedException);
		} else if (errno == EBADF) {
			throw(exc, &SocketConnection_InvalidFileDescriptorException);
		} else {
			throw(exc, &SocketConnection_UnknownErrorException);
		}
	}

	return res;
}

void SocketConnection_Close(SocketConnection *this) {
	if (this->closable) {
		long args[] = { this->fd, SHUT_RDWR };
		syscall(__NR_socketcall, SYS_SHUTDOWN, args);

		syscall(__NR_close, this->fd);
	}
}
