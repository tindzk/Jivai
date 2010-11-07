#import "Time.h"

static ExceptionManager *exc;

void Time0(ExceptionManager *e) {
	exc = e;
}

sdef(self, Empty) {
	self res;

	res.hour   = 0;
	res.minute = 0;
	res.second = 0;

	return res;
}

sdef(short, Compare, self a, self b) {
	short hour = Integer_Compare(a.hour, b.hour);

	if (hour != 0) {
		return hour;
	}

	short minute = Integer_Compare(a.minute, b.minute);

	if (minute != 0) {
		return minute;
	}

	return Integer_Compare(a.second, b.second);
}

inline sdef(bool, Equals, self a, self b) {
	return scall(Compare, a, b) == 0;
}

sdef(ref(UnixEpoch), GetCurrentUnixTime) {
	ref(UnixEpoch) time;

	if (!Kernel_clock_gettime(CLOCK_REALTIME, &time)) {
		throw(exc, excGetTimeOfDayFailed);
	}

	return time;
}
