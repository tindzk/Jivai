#ifndef TIME_H
#define TIME_H

#include <time.h>
#include <sys/syscall.h>

#include "String.h"
#include "Exception.h"

Exception_Export(Time_GetTimeOfDayFailedException);

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

#endif
