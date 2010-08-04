#ifndef DATE_H
#define DATE_H

#include <time.h>
#include <sys/syscall.h>

#include "Integer.h"
#include "Exception.h"

#define Date_Define() \
	uint16_t year;    \
	uint8_t  month;   \
	uint8_t  day;     \
	uint8_t  hour;    \
	uint8_t  minute;  \
	uint8_t  second

typedef struct {
	Date_Define();
} Date;

typedef struct {
	time_t sec;
	long   nsec;
} Date_UnixTime;

#define Date_UnixEpoch \
	{1970, 1, 1, 0, 0, 0}

typedef enum {
	Date_Month_Unset = 0,
	Date_Month_January,
	Date_Month_Februray,
	Date_Month_March,
	Date_Month_April,
	Date_Month_May,
	Date_Month_June,
	Date_Month_July,
	Date_Month_August,
	Date_Month_September,
	Date_Month_October,
	Date_Month_November,
	Date_Month_December
} Date_Month;

typedef enum {
	Date_WeekDay_Sunday = 0,
	Date_WeekDay_Monday,
	Date_WeekDay_Tuesday,
	Date_WeekDay_Wednesday,
	Date_WeekDay_Thursday,
	Date_WeekDay_Friday,
	Date_WeekDay_Saturday,
	Date_WeekDay_Unset
} Date_WeekDay;

#define Date_SecondsMinute (60)
#define Date_SecondsHour   (Date_SecondsMinute *  60)
#define Date_SecondsDay    (Date_SecondsHour   *  24)
#define Date_SecondsYear   (Date_SecondsDay    * 365)

Exception_Export(Date_GetTimeOfDayFailedException);
Exception_Export(Date_YearLower1970Exception);

const short Date_DaysPerMonth[13];

void Date0(ExceptionManager *e);
void Date_Init(Date *this);
bool Date_IsLeap(int year);
uint64_t Date_ToUnixEpoch(Date *this);
Date Date_FromUnixEpoch(uint64_t time);
int Date_Compare(Date a, Date b);
bool Date_Equals(Date a, Date b);
Date_UnixTime Date_GetCurrentUnixTime(void);
Date Date_GetCurrent(void);

#endif
