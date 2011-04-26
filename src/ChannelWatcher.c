#import "ChannelWatcher.h"

#define self ChannelWatcher

rsdef(self, New) {
	int id = Kernel_epoll_create(ref(NumEvents));

	if (id == -1) {
		throw(UnknownError);
	}

	Channel ch = Channel_New(id, 0);
	Channel_SetCloseOnExec(&ch, true);

	return (self) {
		.ch = ch
	};
}

def(void, Destroy) {
	Channel_Destroy(&this->ch);
}

def(void, Subscribe, Channel *ch, int events, GenericInstance inst) {
	EpollEvent ev = {
		.ptr    = Generic_GetObject(inst),
		.events = events
	};

	errno = 0;

	if (!Kernel_epoll_ctl(Channel_GetId(&this->ch), EpollCtl_Add, Channel_GetId(ch), &ev)) {
		if (errno == EPERM) {
			throw(ChannelNotSupported);
		} else if (errno == EEXIST) {
			throw(ChannelAlreadyAdded);
		} else if (errno == EBADF) {
			throw(InvalidChannel);
		} else {
			throw(UnknownError);
		}
	}
}

def(void, Modify, Channel *ch, int events, GenericInstance inst) {
	EpollEvent ev = {
		.ptr    = Generic_GetObject(inst),
		.events = events
	};

	errno = 0;

	if (!Kernel_epoll_ctl(Channel_GetId(&this->ch), EpollCtl_Modify, Channel_GetId(ch), &ev)) {
		if (errno == ENOENT) {
			throw(UnknownChannel);
		} else if (errno == EPERM) {
			throw(ChannelNotSupported);
		} else if (errno == EBADF) {
			throw(InvalidChannel);
		} else {
			throw(UnknownError);
		}
	}
}

def(void, Unsubscribe, Channel *ch) {
	errno = 0;

	if (!Kernel_epoll_ctl(Channel_GetId(&this->ch), EpollCtl_Delete, Channel_GetId(ch), NULL)) {
		if (errno == ENOENT) {
			throw(UnknownChannel);
		} else if (errno == EPERM) {
			throw(ChannelNotSupported);
		} else if (errno == EBADF) {
			throw(InvalidChannel);
		} else {
			throw(UnknownError);
		}
	}
}

def(size_t, Poll, ref(OnEvent) onEvent, int timeout) {
	errno = 0;

	ssize_t nfds;

	if ((nfds = Kernel_epoll_wait(Channel_GetId(&this->ch), this->events, ref(NumEvents), timeout)) == -1) {
		if (errno == EBADF) {
			throw(InvalidChannel);
		} else {
			throw(UnknownError);
		}
	}

	for (size_t i = 0; i < (size_t) nfds; i++) {
		callback(onEvent,
			this->events[i].ptr,
			this->events[i].events);
	}

	return nfds;
}
