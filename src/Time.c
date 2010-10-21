#import "Time.h"

static ExceptionManager *exc;

void Time0(ExceptionManager *e) {
	exc = e;
}

void Time_Init(Time *this) {
	this->hour   = 0;
	this->minute = 0;
	this->second = 0;
}

short Time_Compare(Time a, Time b) {
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

inline bool Time_Equals(Time a, Time b) {
	return Time_Compare(a, b) == 0;
}

Time_UnixEpoch Time_GetCurrentUnixTime(void) {
	Time_UnixEpoch time;

	if (!Kernel_clock_gettime(CLOCK_REALTIME, &time)) {
		throw(exc, excGetTimeOfDayFailed);
	}

	return time;
}
