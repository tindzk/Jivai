#import "Date.h"
#import "Time.h"
#import "String.h"
#import "Exception.h"

#define self DateTime

class {
	Date date;
	Time time;
};

// @exc YearLower1970

rsdef(self, New);
rsdef(self, FromDate, Date date);
sdef(short, Compare, self a, self b);
sdef(self, Drift, self dateTime, s8 hrs, s8 mins);
sdef(self, FromUnixEpoch, u64 time);
sdef(u64, ToUnixEpoch, self dateTime);
sdef(self, GetCurrent);
rsdef(String, Format, self dt);

static alwaysInline sdef(bool, Equals, self a, self b) {
	return Date_Equals(a.date, b.date)
		&& Time_Equals(a.time, b.time);
}

static inline sdef(self, GetUnixEpoch) {
	return (DateTime) {
		.date = {
			.year  = 1970,
			.month = 1,
			.day   = 1
		},

		.time = {
			.hour   = 0,
			.minute = 0,
			.second = 0
		}
	};
}

#undef self
