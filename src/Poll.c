#include "Poll.h"

static ExceptionManager *exc;

Exception_Define(Poll_FileDescriptorAlreadyAddedException);
Exception_Define(Poll_FileDescriptorNotSupportedException);
Exception_Define(Poll_InvalidFileDescriptorException);
Exception_Define(Poll_SettingCloexecFailedException);
Exception_Define(Poll_UnknownErrorException);
Exception_Define(Poll_UnknownFileDescriptorException);

void Poll0(ExceptionManager *e) {
	exc = e;
}

void Poll_Init(Poll *this, size_t maxfds, Poll_OnEvent onEvent, void *context) {
	if ((this->fd = epoll_create(maxfds)) < 0) {
		throw(exc, &Poll_UnknownErrorException);
	}

	if (fcntl(this->fd, F_SETFD, FD_CLOEXEC) < 0) {
		throw(exc, &Poll_SettingCloexecFailedException);
	}

	this->events = (struct epoll_event *) Memory_Alloc(maxfds * sizeof(struct epoll_event));
	this->maxfds = maxfds;
	this->onEvent = onEvent;
	this->context = context;
}

void Poll_Destroy(Poll *this) {
	close(this->fd);
	Memory_Free(this->events);
}

void Poll_AddEvent(Poll *this, void *ptr, int fd, int events) {
	struct epoll_event ev = {0, {0}};

	ev.events = events;
	ev.data.ptr = ptr;

	errno = 0;

	if (epoll_ctl(this->fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
		if (errno == EPERM) {
			throw(exc, &Poll_FileDescriptorNotSupportedException);
		} else if (errno == EEXIST) {
			throw(exc, &Poll_FileDescriptorAlreadyAddedException);
		} else if (errno == EBADF) {
			throw(exc, &Poll_InvalidFileDescriptorException);
		} else {
			throw(exc, &Poll_UnknownErrorException);
		}
	}
}

void Poll_ModifyEvent(Poll *this, void *ptr, int fd, int events) {
	struct epoll_event ev = {0, {0}};

	ev.events = events;
	ev.data.ptr = ptr;

	errno = 0;

	if (epoll_ctl(this->fd, EPOLL_CTL_MOD, fd, &ev) < 0) {
		if (errno == ENOENT) {
			throw(exc, &Poll_UnknownFileDescriptorException);
		} else if (errno == EPERM) {
			throw(exc, &Poll_FileDescriptorNotSupportedException);
		} else if (errno == EBADF) {
			throw(exc, &Poll_InvalidFileDescriptorException);
		} else {
			throw(exc, &Poll_UnknownErrorException);
		}
	}
}

void Poll_DeleteEvent(Poll *this, int fd) {
	errno = 0;

	if (epoll_ctl(this->fd, EPOLL_CTL_DEL, fd, NULL) < 0) {
		if (errno == ENOENT) {
			throw(exc, &Poll_UnknownFileDescriptorException);
		} else if (errno == EPERM) {
			throw(exc, &Poll_FileDescriptorNotSupportedException);
		} else if (errno == EBADF) {
			throw(exc, &Poll_InvalidFileDescriptorException);
		} else {
			throw(exc, &Poll_UnknownErrorException);
		}
	}
}

size_t Poll_Process(Poll *this, int timeout) {
	errno = 0;

	ssize_t nfds;

	if ((nfds = epoll_wait(this->fd, this->events, this->maxfds, timeout)) < 0) {
		if (errno == EBADF) {
			throw(exc, &Poll_InvalidFileDescriptorException);
		} else {
			throw(exc, &Poll_UnknownErrorException);
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
