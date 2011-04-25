#import "Task.h"
#import "Kernel.h"
#import "EventLoop.h"

#import <sys/timerfd.h>

#define self Timer

// @exc UnknownError

Callback(ref(OnTimer), bool, u64 dropped);

class {
	Channel ch;
	bool interval;
};

record(ref(Task)) {
	ref(OnTimer) cb;
	EventLoop_Entry *entry;
	Timer timer;
};

rsdef(self, New, ClockType type);
def(void, Destroy);
def(void, SetTimer, int sec);
def(void, SetInterval, int sec);
rdef(u64, Read);
def(Task *, AsTask, ref(OnTimer) onTimer);

static alwaysInline rdef(bool, IsInterval) {
	return this->interval;
}

#undef self
