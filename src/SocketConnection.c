#import "SocketConnection.h"

#define self SocketConnection

rsdef(self, New, Channel ch, NetworkAddress addr, bool closable) {
	return (self) {
		.ch       = ch,
		.addr     = addr,
		.closable = closable,
		.blocking = true
	};
}

def(void, Destroy) {
	if (this->closable) {
		Kernel_shutdown(Channel_GetId(&this->ch), SHUT_RDWR);
		Channel_Destroy(&this->ch);
	}
}

def(void, SetCorking, bool value) {
	this->corking = value;
}

def(void, SetBlocking, bool value) {
	this->blocking = value;
}

def(void, Flush) {
	if (this->corking) {
		int state = 0;
		Kernel_setsockopt(Channel_GetId(&this->ch), IPPROTO_TCP, TCP_CORK, &state, sizeof(state));
	}
}

def(ssize_t, Read, void *buf, size_t len) {
	int flags = !this->blocking ? MSG_DONTWAIT : 0;

	errno = 0;

	ssize_t res = Kernel_recv(Channel_GetId(&this->ch), buf, len, flags);

	if (res == -1) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			return -1;
		} else if (errno == ECONNRESET) {
			throw(ConnectionReset);
		} else if (errno == ECONNREFUSED) {
			throw(ConnectionRefused);
		} else if (errno == ENOTCONN) {
			throw(NotConnected);
		} else if (errno == EBADF) {
			throw(InvalidFileDescriptor);
		} else {
			throw(UnknownError);
		}
	}

	return res;
}

def(bool, SendFile, File *file, u64 *offset, size_t len) {
	bool isBlocking = Channel_IsBlocking(&this->ch);

	if (!this->blocking) {
		/* sendfile64() does not have an additional flags parameter like send()
		 * which could take MSG_DONTWAIT when blocking is disabled. Therefore
		 * we need to set the blocking mode on the channel persistently.
		 */
		Channel_SetBlocking(&this->ch, this->blocking);
	}

	while (len > 0) {
		size_t write = (len > ref(ChunkSize))
			? ref(ChunkSize)
			: len;

		ssize_t res;

		do {
			errno = 0;
			res = Kernel_sendfile64(
				Channel_GetId(&this->ch),
				Channel_GetId(&file->ch),
				offset, write);
		} while (res == -1 && errno == EINTR);

		if (res == -1) {
			if (errno == EAGAIN) {
				return false;
			} else if (errno == EBADF) {
				throw(InvalidFileDescriptor);
			} else if (errno == EINVAL) {
				throw(FileDescriptorUnusable);
			} else {
				throw(UnknownError);
			}
		} else if (res == 0) {
			break;
		}

		len -= res;
	}

	/* Restore previous blocking status. */
	Channel_SetBlocking(&this->ch, isBlocking);

	return true;
}

overload def(ssize_t, Write, void *buf, size_t len) {
	int flags = MSG_NOSIGNAL;

	if (this->corking) {
		flags |= MSG_MORE;
	}

	if (!this->blocking) {
		flags |= MSG_DONTWAIT;
	}

	errno = 0;

	ssize_t res = Kernel_send(Channel_GetId(&this->ch), buf, len, flags);

	if (res == -1) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			return -1;
		} else if (errno == ECONNRESET) {
			throw(ConnectionReset);
		} else if (errno == EPIPE || errno == ENOTCONN) {
			throw(NotConnected);
		} else if (errno == EBADF) {
			throw(InvalidFileDescriptor);
		} else {
			throw(UnknownError);
		}
	}

	return res;
}
