#import <errno.h>

#import "Types.h"
#import "Kernel.h"
#import "String.h"
#import "Memory.h"
#import "Exception.h"

#define self Poll

#ifndef Poll_NumEvents
#define Poll_NumEvents 4096
#endif

set(ref(Events)) {
    ref(Events_Input)          = 0x001,
    ref(Events_Priority)       = 0x002,
    ref(Events_Output)         = 0x004,
    ref(Events_ReadNormal)     = 0x040,
    ref(Events_ReadOutOfBand)  = 0x080,
    ref(Events_WriteNormal)    = 0x100,
    ref(Events_WriteOutOfBand) = 0x200,
    ref(Events_Message)        = 0x400,
    ref(Events_Error)          = 0x008,
    ref(Events_HangUp)         = 0x010,
    ref(Events_PeerHangUp)     = 0x2000,
    ref(Events_OneShot)        = (1 << 30),
    ref(Events_EdgeTriggered)  = (1 << 31)
};

Callback(ref(OnEvent), void, int events, GenericInstance inst);

class {
	int fd;
	EpollEvent events[ref(NumEvents)];
	ref(OnEvent) onEvent;
};

// @exc FileDescriptorAlreadyAdded
// @exc FileDescriptorNotSupported
// @exc SettingCloexecFailed
// @exc UnknownFileDescriptor
// @exc UnknownError
// @exc InvalidFileDescriptor

def(void, Init, ref(OnEvent) onEvent);
def(void, Destroy);
def(void, AddEvent, GenericInstance inst, int fd, int events);
def(void, ModifyEvent, GenericInstance inst, int fd, int events);
def(void, DeleteEvent, int fd);
def(size_t, Process, int timeout);

#undef self
