#import "Kernel.h"
#import "Number.h"
#import "Integer.h"
#import "Exception.h"

#define self Time

// @exc GetTimeOfDayFailed

record(self) {
	u8 hour;
	u8 minute;
	u8 second;
};

sdef(self, Empty);
sdef(short, Compare, self a, self b);
sdef(bool, Equals, self a, self b);
sdef(ref(UnixEpoch), GetCurrent);
sdef(String, Format, self time);

#undef self
