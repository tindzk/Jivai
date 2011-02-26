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

sdef(ref(UnixEpoch), GetCurrent) {
	ref(UnixEpoch) time;

	if (!Kernel_clock_gettime(CLOCK_REALTIME, &time)) {
		throw(GetTimeOfDayFailed);
	}

	return time;
}

sdef(String, Format, self time) {
	String hour   = Number_Format(time.hour,   2);
	String minute = Number_Format(time.minute, 2);
	String second = Number_Format(time.second, 2);

	String fmt = String_Format($("%:%:%"),
		hour.prot, minute.prot, second.prot);

	String_Destroy(&second);
	String_Destroy(&minute);
	String_Destroy(&hour);

	return fmt;
}
