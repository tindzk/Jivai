#import "Date.h"
#import "Time.h"
#import "String.h"
#import "Exception.h"

#undef self
#define self DateTime

record(self) {
	Date date;
	Time time;
};

enum {
	excYearLower1970 = excOffset
};

void DateTime0(ExceptionManager *e);

sdef(self, Empty);
sdef(self, GetUnixEpoch);
sdef(short, Compare, self a, self b);
sdef(bool, Equals, self a, self b);
sdef(self, FromUnixEpoch, u64 time);
sdef(u64, ToUnixEpoch, self dateTime);
sdef(self, GetCurrent);
