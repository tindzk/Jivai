#import "Timer.h"

#define self Timer

rsdef(self, New, ClockType type) {
	assert(type == ClockType_Monotonic || type == ClockType_Realtime);

	self res = {
		.fd = timerfd_create(type, TFD_NONBLOCK)
	};

	if (res.fd == -1) {
		throw(UnknownError);
	}

	return res;
}

def(void, Destroy) {
	Kernel_close(this->fd);
}

def(void, SetTimer, int sec) {
	assert(sec != 0);

	struct itimerspec value = {
		.it_interval.tv_sec = 0,
		.it_value.tv_sec    = sec
	};

	int ret = timerfd_settime(this->fd, 0, &value, NULL);

	if (ret < 0) {
		throw(UnknownError);
	}

	this->interval = false;
}

def(void, SetInterval, int sec) {
	assert(sec != 0);

	struct itimerspec value = {
		.it_interval.tv_sec = sec,
		.it_value.tv_sec    = sec
	};

	int ret = timerfd_settime(this->fd, 0, &value, NULL);

	if (ret < 0) {
		throw(UnknownError);
	}

	this->interval = true;
}

/* Returns the number of expirations that have occurred since the last Read()
 * call.
 */
rdef(u64, Read) {
	u64 dropped;

	int ret = Kernel_read(this->fd, &dropped, sizeof(dropped));

	if (ret < 0) {
		throw(UnknownError);
	}

	return dropped;
}
