#import "Task.h"
#import "Kernel.h"
#import "EventLoop.h"

#import <sys/timerfd.h>

#define self Timer

exc(UnknownError)

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

rsdef(self, new, ClockType type);
def(void, destroy);
overload def(void, setRelative, time_t start, time_t interval);
overload def(void, setAbsolute, time_t start, time_t interval);
rdef(u64, read);
def(Task *, asTask, ref(OnTimer) onTimer);

static alwaysInline rdef(bool, isInterval) {
	return this->interval;
}

/* Register a timer expiring every `sec' seconds, starting `sec' seconds
 * from now.
 */
static alwaysInline def(void, setInterval, int sec) {
	call(setRelative, sec, sec);
}

/* Relative/absolute timers that are not recurring. */
static alwaysInline overload def(void, setRelative, time_t start) {
	call(setRelative, start, 0);
}

static alwaysInline overload def(void, setAbsolute, time_t start) {
	call(setAbsolute, start, 0);
}

#undef self
