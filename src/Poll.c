#import "Poll.h"

size_t Modules_Poll;

static ExceptionManager *exc;

void Poll0(ExceptionManager *e) {
	Modules_Poll = Module_Register(String("Poll"));

	exc = e;
}

void Poll_Init(Poll *this, Poll_OnEvent onEvent, void *context) {
	if ((this->fd = Kernel_epoll_create(Poll_Events)) == -1) {
		throw(exc, excUnknownError);
	}

	if (Kernel_fcntl(this->fd,
		FcntlMode_GetDescriptorFlags,
		FileDescriptorFlags_CloseOnExec) == -1)
	{
		throw(exc, excSettingCloexecFailed);
	}

	this->onEvent = onEvent;
	this->context = context;
}

void Poll_Destroy(Poll *this) {
	Kernel_close(this->fd);
}

void Poll_AddEvent(Poll *this, void *ptr, ssize_t fd, int events) {
	struct epoll_event ev = {0, {0}};

	ev.events = events;
	ev.data.ptr = ptr;

	errno = 0;

	if (!Kernel_epoll_ctl(this->fd, EPOLL_CTL_ADD, fd, &ev)) {
		if (errno == EPERM) {
			throw(exc, excFileDescriptorNotSupported);
		} else if (errno == EEXIST) {
			throw(exc, excFileDescriptorAlreadyAdded);
		} else if (errno == EBADF) {
			throw(exc, excInvalidFileDescriptor);
		} else {
			throw(exc, excUnknownError);
		}
	}
}

void Poll_ModifyEvent(Poll *this, void *ptr, ssize_t fd, int events) {
	struct epoll_event ev = {0, {0}};

	ev.events = events;
	ev.data.ptr = ptr;

	errno = 0;

	if (!Kernel_epoll_ctl(this->fd, EPOLL_CTL_MOD, fd, &ev)) {
		if (errno == ENOENT) {
			throw(exc, excUnknownFileDescriptor);
		} else if (errno == EPERM) {
			throw(exc, excFileDescriptorNotSupported);
		} else if (errno == EBADF) {
			throw(exc, excInvalidFileDescriptor);
		} else {
			throw(exc, excUnknownError);
		}
	}
}

void Poll_DeleteEvent(Poll *this, int fd) {
	errno = 0;

	if (Kernel_epoll_ctl(this->fd, EPOLL_CTL_DEL, fd, NULL)) {
		if (errno == ENOENT) {
			throw(exc, excUnknownFileDescriptor);
		} else if (errno == EPERM) {
			throw(exc, excFileDescriptorNotSupported);
		} else if (errno == EBADF) {
			throw(exc, excInvalidFileDescriptor);
		} else {
			throw(exc, excUnknownError);
		}
	}
}

size_t Poll_Process(Poll *this, int timeout) {
	errno = 0;

	ssize_t nfds;

	if ((nfds = Kernel_epoll_wait(this->fd, this->events, Poll_Events, timeout)) == -1) {
		if (errno == EBADF) {
			throw(exc, excInvalidFileDescriptor);
		} else {
			throw(exc, excUnknownError);
		}
	}

	for (size_t i = 0; i < (size_t) nfds; i++) {
		this->onEvent(
			this->context,
			this->events[i].events,
			this->events[i].data.ptr);
	}

	return nfds;
}
