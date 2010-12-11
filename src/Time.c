#import "Time.h"

#define self Time

sdef(self, Empty) {
	self res;

	res.hour   = 0;
	res.minute = 0;
	res.second = 0;

	return res;
}

sdef(short, Compare, self a, self b) {
	short hour = Int16_Compare(a.hour, b.hour);

	if (hour != 0) {
		return hour;
	}

	short minute = Int16_Compare(a.minute, b.minute);

	if (minute != 0) {
		return minute;
	}

	return Int16_Compare(a.second, b.second);
}

inline sdef(bool, Equals, self a, self b) {
	return scall(Compare, a, b) == 0;
}

sdef(ref(UnixEpoch), GetCurrentUnixTime) {
	ref(UnixEpoch) time;

	if (!Kernel_clock_gettime(CLOCK_REALTIME, &time)) {
		throw(GetTimeOfDayFailed);
	}

	return time;
}
