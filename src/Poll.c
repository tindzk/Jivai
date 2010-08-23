#import "Poll.h"

static ExceptionManager *exc;

Exception_Define(FileDescriptorAlreadyAddedException);
Exception_Define(FileDescriptorNotSupportedException);
Exception_Define(InvalidFileDescriptorException);
Exception_Define(SettingCloexecFailedException);
Exception_Define(UnknownErrorException);
Exception_Define(UnknownFileDescriptorException);

void Poll0(ExceptionManager *e) {
	exc = e;
}

void Poll_Init(Poll *this, Poll_OnEvent onEvent, void *context) {
	if ((this->fd = syscall(__NR_epoll_create, Poll_Events)) < 0) {
		throw(exc, &UnknownErrorException);
	}

	if (syscall(__NR_fcntl, this->fd,
		FcntlMode_GetDescriptorFlags,
		FileDescriptorFlags_CloseOnExec) < 0)
	{
		throw(exc, &SettingCloexecFailedException);
	}

	this->onEvent = onEvent;
	this->context = context;
}

void Poll_Destroy(Poll *this) {
	syscall(__NR_close, this->fd);
}

void Poll_AddEvent(Poll *this, void *ptr, int fd, int events) {
	struct epoll_event ev = {0, {0}};

	ev.events = events;
	ev.data.ptr = ptr;

	errno = 0;

	if (syscall(__NR_epoll_ctl, this->fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
		if (errno == EPERM) {
			throw(exc, &FileDescriptorNotSupportedException);
		} else if (errno == EEXIST) {
			throw(exc, &FileDescriptorAlreadyAddedException);
		} else if (errno == EBADF) {
			throw(exc, &InvalidFileDescriptorException);
		} else {
			throw(exc, &UnknownErrorException);
		}
	}
}

void Poll_ModifyEvent(Poll *this, void *ptr, int fd, int events) {
	struct epoll_event ev = {0, {0}};

	ev.events = events;
	ev.data.ptr = ptr;

	errno = 0;

	if (syscall(__NR_epoll_ctl, this->fd, EPOLL_CTL_MOD, fd, &ev) < 0) {
		if (errno == ENOENT) {
			throw(exc, &UnknownFileDescriptorException);
		} else if (errno == EPERM) {
			throw(exc, &FileDescriptorNotSupportedException);
		} else if (errno == EBADF) {
			throw(exc, &InvalidFileDescriptorException);
		} else {
			throw(exc, &UnknownErrorException);
		}
	}
}

void Poll_DeleteEvent(Poll *this, int fd) {
	errno = 0;

	if (syscall(__NR_epoll_ctl, this->fd, EPOLL_CTL_DEL, fd, NULL) < 0) {
		if (errno == ENOENT) {
			throw(exc, &UnknownFileDescriptorException);
		} else if (errno == EPERM) {
			throw(exc, &FileDescriptorNotSupportedException);
		} else if (errno == EBADF) {
			throw(exc, &InvalidFileDescriptorException);
		} else {
			throw(exc, &UnknownErrorException);
		}
	}
}

size_t Poll_Process(Poll *this, int timeout) {
	errno = 0;

	ssize_t nfds;

	if ((nfds = syscall(__NR_epoll_wait, this->fd, this->events, Poll_Events, timeout)) < 0) {
		if (errno == EBADF) {
			throw(exc, &InvalidFileDescriptorException);
		} else {
			throw(exc, &UnknownErrorException);
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
