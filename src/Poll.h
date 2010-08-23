#import <errno.h>
#import <sys/epoll.h>
#import <sys/syscall.h>

#import "NULL.h"
#import "Fcntl.h"
#import "Memory.h"

#undef self
#define self Poll

/* Needed for compatibility with diet libc. */
#ifndef EPOLLRDHUP
#define EPOLLRDHUP 0x2000
#endif

#ifndef Poll_Events
#define Poll_Events 4096
#endif

typedef void (* Poll_OnEvent)(void *, int, void *);

typedef struct {
	int fd;

	struct epoll_event events[Poll_Events];

	void *context;
	Poll_OnEvent onEvent;
} Poll;

Exception_Export(Poll_FileDescriptorAlreadyAddedException);
Exception_Export(Poll_FileDescriptorNotSupportedException);
Exception_Export(Poll_InvalidFileDescriptorException);
Exception_Export(Poll_SettingCloexecFailedException);
Exception_Export(Poll_UnknownErrorException);
Exception_Export(Poll_UnknownFileDescriptorException);

void Poll0(ExceptionManager *e);

void Poll_Init(Poll *this, Poll_OnEvent onEvent, void *context);
void Poll_Destroy(Poll *this);
void Poll_AddEvent(Poll *this, void *ptr, int fd, int events);
void Poll_ModifyEvent(Poll *this, void *ptr, int fd, int events);
void Poll_DeleteEvent(Poll *this, int fd);
size_t Poll_Process(Poll *this, int timeout);
