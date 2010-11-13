#import "Poll.h"
#import "App.h"

static ExceptionManager *exc;

void Poll0(ExceptionManager *e) {
	exc = e;
}

def(void, Init, ref(OnEvent) onEvent) {
	if ((this->fd = Kernel_epoll_create(ref(NumEvents))) == -1) {
		throw(exc, excUnknownError);
	}

	if (Kernel_fcntl(this->fd,
		FcntlMode_GetDescriptorFlags,
		FileDescriptorFlags_CloseOnExec) == -1)
	{
		throw(exc, excSettingCloexecFailed);
	}

	this->onEvent = onEvent;
}

def(void, Destroy) {
	Kernel_close(this->fd);
}

def(void, AddEvent, GenericInstance inst, ssize_t fd, int events) {
	EpollEvent ev = { 0, {0} };

	ev.ptr    = Generic_GetObject(inst);
	ev.events = events;

	errno = 0;

	if (!Kernel_epoll_ctl(this->fd, EpollCtl_Add, fd, &ev)) {
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

def(void, ModifyEvent, GenericInstance inst, ssize_t fd, int events) {
	EpollEvent ev = { 0, {0} };

	ev.ptr    = Generic_GetObject(inst);
	ev.events = events;

	errno = 0;

	if (!Kernel_epoll_ctl(this->fd, EpollCtl_Modify, fd, &ev)) {
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

def(void, DeleteEvent, int fd) {
	errno = 0;

	if (Kernel_epoll_ctl(this->fd, EpollCtl_Delete, fd, NULL)) {
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

def(size_t, Process, int timeout) {
	errno = 0;

	ssize_t nfds;

	if ((nfds = Kernel_epoll_wait(this->fd, this->events, ref(NumEvents), timeout)) == -1) {
		if (errno == EBADF) {
			throw(exc, excInvalidFileDescriptor);
		} else {
			throw(exc, excUnknownError);
		}
	}

	for (size_t i = 0; i < (size_t) nfds; i++) {
		callback(this->onEvent,
			this->events[i].events,
			this->events[i].ptr);
	}

	return nfds;
}
