#import "Kernel.h"
#import "Integer.h"
#import "Exception.h"

#undef self
#define self Time

#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 0
#endif

enum {
	excGetTimeOfDayFailed = excOffset
};

record(self) {
	u8 hour;
	u8 minute;
	u8 second;
};

void Time0(ExceptionManager *e);

sdef(self, Empty);
sdef(short, Compare, self a, self b);
sdef(bool, Equals, self a, self b);
sdef(ref(UnixEpoch), GetCurrentUnixTime);
