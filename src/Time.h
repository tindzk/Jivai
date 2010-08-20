#import <time.h>
#import <sys/syscall.h>

#import "String.h"
#import "Exception.h"

#undef self
#define self Time

Exception_Export(GetTimeOfDayFailedException);

typedef struct {
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} Time;

typedef struct {
	time_t sec;
	long   nsec;
} Time_UnixEpoch;

void Time0(ExceptionManager *e);
void Time_Init(Time *this);
short Time_Compare(Time a, Time b);
bool Time_Equals(Time a, Time b);
Time_UnixEpoch Time_GetCurrentUnixTime(void);
