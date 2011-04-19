#import "Time.h"

#define self Time

rsdef(self, New) {
	return (self) {
		.hour   = 0,
		.minute = 0,
		.second = 0
	};
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

sdef(ref(UnixEpoch), GetCurrent) {
	ref(UnixEpoch) time;

	if (!Kernel_clock_gettime(ClockType_Realtime, &time)) {
		throw(GetTimeOfDayFailed);
	}

	return time;
}

sdef(String, Format, self time) {
	String hour   = Number_Format(time.hour,   2);
	String minute = Number_Format(time.minute, 2);
	String second = Number_Format(time.second, 2);

	String fmt = String_Format($("%:%:%"),
		hour.rd, minute.rd, second.rd);

	String_Destroy(&second);
	String_Destroy(&minute);
	String_Destroy(&hour);

	return fmt;
}
