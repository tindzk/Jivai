#import "SocketConnection.h"
#import "App.h"

def(void, Flush) {
	if (this->corking) {
		int state = 0;
		Kernel_setsockopt(this->fd, IPPROTO_TCP, TCP_CORK, &state, sizeof(state));
	}
}

def(ssize_t, Read, void *buf, size_t len) {
	int flags = this->nonblocking ? MSG_DONTWAIT : 0;

	errno = 0;

	ssize_t res = Kernel_recv(this->fd, buf, len, flags);

	if (res == -1) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			return -1;
		} else if (errno == ECONNRESET) {
			throw(excConnectionReset);
		} else if (errno == ECONNREFUSED) {
			throw(excConnectionRefused);
		} else if (errno == ENOTCONN) {
			throw(excNotConnected);
		} else if (errno == EBADF) {
			throw(excInvalidFileDescriptor);
		} else {
			throw(excUnknownError);
		}
	}

	return res;
}

def(bool, SendFile, File *file, u64 *offset, size_t len) {
	if (this->nonblocking) {
		if (Kernel_fcntl(this->fd, FcntlMode_SetStatus,
			FileStatus_ReadWrite | FileStatus_NonBlock) == -1)
		{
			throw(excFcntlFailed);
		}
	}

	while (len > 0) {
		size_t write = (len > ref(ChunkSize))
			? ref(ChunkSize)
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
				throw(excInvalidFileDescriptor);
			} else if (errno == EINVAL) {
				throw(excFileDescriptorUnusable);
			} else {
				throw(excUnknownError);
			}
		} else if (res == 0) {
			break;
		}

		len -= res;
	}

	if (this->nonblocking) {
		if (Kernel_fcntl(this->fd, FcntlMode_SetStatus, FileStatus_ReadWrite) == -1) {
			throw(excFcntlFailed);
		}
	}

	return true;
}

def(ssize_t, Write, void *buf, size_t len) {
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
			throw(excConnectionReset);
		} else if (errno == EPIPE || errno == ENOTCONN) {
			throw(excNotConnected);
		} else if (errno == EBADF) {
			throw(excInvalidFileDescriptor);
		} else {
			throw(excUnknownError);
		}
	}

	return res;
}

def(void, Close) {
	if (this->closable) {
		Kernel_shutdown(this->fd, SHUT_RDWR);
		Kernel_close(this->fd);
	}
}
