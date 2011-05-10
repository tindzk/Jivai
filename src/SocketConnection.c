#import "SocketConnection.h"

#define self SocketConnection

rsdef(self, New, Channel *ch, NetworkAddress addr) {
	return (self) {
		.ch   = *ch,
		.addr = addr
	};
}

def(void, Destroy) {
	Kernel_shutdown(Channel_GetId(&this->ch), SHUT_RDWR);
	Channel_Destroy(&this->ch);
}

def(void, SetCorking, bool value) {
	this->corking = value;
}

def(void, Flush) {
	assert(this->corking);

	int state = 0;

	Kernel_setsockopt(Channel_GetId(&this->ch), IPPROTO_TCP, TCP_CORK,
		&state, sizeof(state));
}

def(ssize_t, Read, void *buf, size_t len) {
	ssize_t res = Kernel_recv(Channel_GetId(&this->ch), buf, len, 0);

	if (res == -1) {
		if (errno == EAGAIN) {
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
	while (len > 0) {
		size_t write = (len > ref(ChunkSize))
			? ref(ChunkSize)
			: len;

		ssize_t res;

		do {
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

	return true;
}

overload def(ssize_t, Write, void *buf, size_t len) {
	int flags = MSG_NOSIGNAL;

	if (this->corking) {
		flags |= MSG_MORE;
	}

	ssize_t res = Kernel_send(Channel_GetId(&this->ch), buf, len, flags);

	if (res == -1) {
		if (errno == EAGAIN) {
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
