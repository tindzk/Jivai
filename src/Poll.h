#import <errno.h>
#import <sys/epoll.h>

#import "Types.h"
#import "Fcntl.h"
#import "Kernel.h"
#import "String.h"
#import "Memory.h"
#import "Exception.h"

#undef self
#define self Poll

/* Needed for compatibility with diet libc. */
#ifndef EPOLLRDHUP
#define EPOLLRDHUP 0x2000
#endif

#ifndef Poll_Events
#define Poll_Events 4096
#endif

DefineCallback(ref(OnEvent), void, int events, GenericInstance inst);

class(self) {
	ssize_t fd;
	struct epoll_event events[ref(Events)];
	ref(OnEvent) onEvent;
};

enum {
	excFileDescriptorAlreadyAdded = excOffset,
	excFileDescriptorNotSupported,
	excSettingCloexecFailed,
	excUnknownFileDescriptor
};

void Poll0(ExceptionManager *e);

def(void, Init, ref(OnEvent) onEvent);
def(void, Destroy);
def(void, AddEvent, GenericInstance inst, ssize_t fd, int events);
def(void, ModifyEvent, GenericInstance inst, ssize_t fd, int events);
def(void, DeleteEvent, int fd);
def(size_t, Process, int timeout);
