#import "Date.h"
#import "Time.h"
#import "String.h"
#import "Exception.h"

#define self DateTime

record(self) {
	Date date;
	Time time;
};

// @exc YearLower1970

sdef(self, Empty);
sdef(self, FromDate, Date date);
sdef(self, GetUnixEpoch);
sdef(short, Compare, self a, self b);
sdef(self, Drift, self dateTime, s8 hrs, s8 mins);
sdef(bool, Equals, self a, self b);
sdef(self, FromUnixEpoch, u64 time);
sdef(u64, ToUnixEpoch, self dateTime);
sdef(self, GetCurrent);
sdef(String, Format, self dt);

#undef self
