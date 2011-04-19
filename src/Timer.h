#import "Kernel.h"

#import <sys/timerfd.h>

#define self Timer

// @exc UnknownError

class {
	int fd;
	bool interval;
};

rsdef(self, New, ClockType type);
def(void, Destroy);
def(void, SetTimer, int sec);
def(void, SetInterval, int sec);
rdef(u64, Read);

static alwaysInline rdef(bool, IsInterval) {
	return this->interval;
}

#undef self
