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
sdef(ref(UnixEpoch), GetCurrent);
sdef(String, Format, self time);

static alwaysInline sdef(bool, Equals, self a, self b) {
	return scall(Compare, a, b) == 0;
}

#undef self
