#import "Date.h"
#import "Time.h"
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

Exception_Export(YearLower1970Exception);

void DateTime0(ExceptionManager *e);
void DateTime_Init(DateTime *this);
short DateTime_Compare(DateTime a, DateTime b);
bool DateTime_Equals(DateTime a, DateTime b);
DateTime DateTime_FromUnixEpoch(uint64_t time);
uint64_t DateTime_ToUnixEpoch(DateTime *this);
DateTime DateTime_GetCurrent(void);
