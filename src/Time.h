#import "Kernel.h"
#import "String.h"
#import "Exception.h"

#undef self
#define self Time

#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 0
#endif

enum {
	excGetTimeOfDayFailed = excOffset
};

typedef struct {
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} Time;

void Time0(ExceptionManager *e);
void Time_Init(Time *this);
short Time_Compare(Time a, Time b);
bool Time_Equals(Time a, Time b);
Time_UnixEpoch Time_GetCurrentUnixTime(void);
