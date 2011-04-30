#import "Types.h"
#import "Kernel.h"
#import "Memory.h"
#import "Channel.h"
#import "Exception.h"

#define self ChannelWatcher

#ifndef ChannelWatcher_NumEvents
#define ChannelWatcher_NumEvents 4096
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

Callback(ref(OnEvent), void, void *addr, int events);

class {
	Channel ch;
	EpollEvent events[ref(NumEvents)];
};

// @exc ChannelAlreadyAdded
// @exc ChannelNotSupported
// @exc InvalidChannel
// @exc SettingCloexecFailed
// @exc UnknownChannel
// @exc UnknownError

rsdef(self, New);
def(void, Destroy);
def(void, Subscribe, Channel *ch, int events, void *addr);
def(void, Modify, Channel *ch, int events, void *addr);
def(void, Unsubscribe, Channel *ch);
def(size_t, Poll, ref(OnEvent) onEvent, int timeout);

#undef self
