#import "SocketConnection.h"

size_t Modules_SocketConnection;

static ExceptionManager *exc;

void SocketConnection0(ExceptionManager *e) {
	Modules_SocketConnection =
		Module_Register(String("SocketConnection"));

	exc = e;
}

void SocketConnection_Flush(SocketConnection *this) {
	if (this->corking) {
		int state = 0;
		Kernel_setsockopt(this->fd, IPPROTO_TCP, TCP_CORK, &state, sizeof(state));
	}
}

ssize_t SocketConnection_Read(SocketConnection *this, void *buf, size_t len) {
	int flags = this->nonblocking ? MSG_DONTWAIT : 0;

	errno = 0;

	ssize_t res = Kernel_recv(this->fd, buf, len, flags);

	if (res == -1) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			return -1;
		} else if (errno == ECONNRESET) {
			throw(exc, excConnectionReset);
		} else if (errno == ECONNREFUSED) {
			throw(exc, excConnectionRefused);
		} else if (errno == ENOTCONN) {
			throw(exc, excNotConnected);
		} else if (errno == EBADF) {
			throw(exc, excInvalidFileDescriptor);
		} else {
			throw(exc, excUnknownError);
		}
	}

	return res;
}

bool SocketConnection_SendFile(SocketConnection *this, File *file, u64 *offset, size_t len) {
	if (this->nonblocking) {
		if (Kernel_fcntl(this->fd, FcntlMode_SetStatus,
			FileStatus_ReadWrite | FileStatus_NonBlock) == -1)
		{
			throw(exc, excFcntlFailed);
		}
	}

	while (len > 0) {
		size_t write = (len > SocketConnection_ChunkSize)
			? SocketConnection_ChunkSize
			: len;

		ssize_t res;

		do {
			errno = 0;
			res = Kernel_sendfile64(this->fd, file->fd, offset, write);
		} while (res == -1 && errno == EINTR);

		if (res == -1) {
			if (errno == EAGAIN) {
				return false;
			} else if (errno == EBADF) {
				throw(exc, excInvalidFileDescriptor);
			} else if (errno == EINVAL) {
				throw(exc, excFileDescriptorUnusable);
			} else {
				throw(exc, excUnknownError);
			}
		} else if (res == 0) {
			break;
		}

		len -= res;
	}

	if (this->nonblocking) {
		if (Kernel_fcntl(this->fd, FcntlMode_SetStatus, FileStatus_ReadWrite) == -1) {
			throw(exc, excFcntlFailed);
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

	ssize_t res = Kernel_send(this->fd, buf, len, flags);

	if (res == -1) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			return -1;
		} else if (errno == ECONNRESET) {
			throw(exc, excConnectionReset);
		} else if (errno == EPIPE || errno == ENOTCONN) {
			throw(exc, excNotConnected);
		} else if (errno == EBADF) {
			throw(exc, excInvalidFileDescriptor);
		} else {
			throw(exc, excUnknownError);
		}
	}

	return res;
}

void SocketConnection_Close(SocketConnection *this) {
	if (this->closable) {
		Kernel_shutdown(this->fd, SHUT_RDWR);
		Kernel_close(this->fd);
	}
}
