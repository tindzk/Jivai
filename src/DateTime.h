#import "Date.h"
#import "Time.h"
#import "String.h"
#import "Exception.h"

#undef self
#define self DateTime

typedef struct {
	Date date;
	Time time;
} DateTime;

#define DateTime_UnixEpoch \
	(DateTime) {           \
		.date = {          \
			.year  = 1970, \
			.month = 1,    \
			.day   = 1     \
		},                 \
                           \
		.time = {          \
			.hour   = 0,   \
			.minute = 0,   \
			.second = 0    \
		}                  \
	}

enum {
	excYearLower1970 = excOffset
};

void DateTime0(ExceptionManager *e);
void DateTime_Init(DateTime *this);
short DateTime_Compare(DateTime a, DateTime b);
bool DateTime_Equals(DateTime a, DateTime b);
DateTime DateTime_FromUnixEpoch(u64 time);
u64 DateTime_ToUnixEpoch(DateTime *this);
DateTime DateTime_GetCurrent(void);
