#import <errno.h>
#import <sys/epoll.h>

#import "Types.h"
#import "Fcntl.h"
#import "Kernel.h"
#import "String.h"
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
	ssize_t fd;

	struct epoll_event events[Poll_Events];

	void *context;
	Poll_OnEvent onEvent;
} Poll;

enum {
	excFileDescriptorAlreadyAdded = excOffset,
	excFileDescriptorNotSupported,
	excSettingCloexecFailed,
	excUnknownFileDescriptor
};

extern size_t Modules_Poll;

void Poll0(ExceptionManager *e);

void Poll_Init(Poll *this, Poll_OnEvent onEvent, void *context);
void Poll_Destroy(Poll *this);
void Poll_AddEvent(Poll *this, void *ptr, ssize_t fd, int events);
void Poll_ModifyEvent(Poll *this, void *ptr, ssize_t fd, int events);
void Poll_DeleteEvent(Poll *this, int fd);
size_t Poll_Process(Poll *this, int timeout);
