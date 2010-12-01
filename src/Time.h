#import "Kernel.h"
#import "Integer.h"
#import "Exception.h"

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

sdef(self, Empty);
sdef(short, Compare, self a, self b);
sdef(bool, Equals, self a, self b);
sdef(ref(UnixEpoch), GetCurrentUnixTime);

#undef self
