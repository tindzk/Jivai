#include "Date.h"

Exception_Define(Date_GetTimeOfDayFailedException);
Exception_Define(Date_YearLower1970Exception);

/* Contains the days up until the end of a month. Refers to a non-leap year.
 * Taken from diet libc (dietlibc-0.32/libugly/time_table_spd.c).
 */
const short Date_DaysPerMonth[13] = {
	0,
	(31),
	(31 + 28),
	(31 + 28 + 31),
	(31 + 28 + 31 + 30),
	(31 + 28 + 31 + 30 + 31),
	(31 + 28 + 31 + 30 + 31 + 30),
	(31 + 28 + 31 + 30 + 31 + 30 + 31),
	(31 + 28 + 31 + 30 + 31 + 30 + 31 + 31),
	(31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30),
	(31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31),
	(31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30),
	(31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31)
};

static ExceptionManager *exc;

void Date0(ExceptionManager *e) {
	exc = e;
}

void Date_Init(Date *this) {
	this->year   = 0;
	this->month  = 0;
	this->day    = 0;
	this->hour   = 0;
	this->minute = 0;
	this->second = 0;
}

/* Taken from diet libc (dietlibc-0.32/libugly/isleap.c) */
bool Date_IsLeap(int year) {
	/* Every fourth year is a leap year except for century years that are
	 * not divisible by 400.
	 */
	return (!(year % 4) && ((year % 100) || !(year % 400)));
}

uint64_t Date_ToUnixEpoch(Date *this) {
	if (this->year < 1970) {
		throw(exc, &Date_YearLower1970Exception);
	}

	short years = this->year - 1970;

	/* Days since 1970. */
	int days = years * 365;

	/* Add one day for each leap year since 1970. */
	days += (years + 1) / 4;

	/* Add one extra day when the year is a leap year and the January is already over. */
	if (this->month > 1) {
		if (Date_IsLeap(this->year)) {
			days++;
		}
	}

	/* Add number of days up until the end of the current month. */
	days += Date_DaysPerMonth[this->month - 1] + this->day - 1;

	uint64_t hours   = days    * 24 + this->hour;
	uint64_t minutes = hours   * 60 + this->minute;
	uint64_t seconds = minutes * 60 + this->second;

	return seconds;
}

Date Date_FromUnixEpoch(uint64_t time) {
	Date res;

	res.year = (int)(time / Date_SecondsYear);
	time -= (int)(res.year * Date_SecondsYear);

	res.day = (int)(time / Date_SecondsDay);
	time -= (int)(res.day * Date_SecondsDay);

	res.hour = (int)(time / Date_SecondsHour);
	time -= (int)(res.hour * Date_SecondsHour);

	res.minute = (int)(time / Date_SecondsMinute);
	time -= (int)(res.minute * Date_SecondsMinute);

	res.second = time;

	/* Subtract one day for each leap year. */
	res.day -= (res.year + 1) / 4;

	/* The day has already begun. */
	res.day++;

	res.month = 0;

	for (size_t i = 12; i >= 1; i--) {
		if (res.day / Date_DaysPerMonth[i]) {
			res.day -= Date_DaysPerMonth[i];
			res.month = i;

			break;
		}
	}

	res.month++;

	res.year += 1970;

	return res;
}

int Date_Compare(Date a, Date b) {
	int year = Integer_Compare(a.year, b.year);

	if (year != 0) {
		return year;
	}

	int month = Integer_Compare(a.month, b.month);

	if (month != 0) {
		return month;
	}

	int day = Integer_Compare(a.day, b.day);

	if (day != 0) {
		return day;
	}

	int hour = Integer_Compare(a.hour, b.hour);

	if (hour != 0) {
		return hour;
	}

	int minute = Integer_Compare(a.minute, b.minute);

	if (minute != 0) {
		return minute;
	}

	return Integer_Compare(a.second, b.second);
}

inline bool Date_Equals(Date a, Date b) {
	return Date_Compare(a, b) == 0;
}

Date_UnixTime Date_GetCurrentUnixTime(void) {
	Date_UnixTime time;

	if (syscall(__NR_clock_gettime, CLOCK_REALTIME, &time) < 0) {
		throw(exc, &Date_GetTimeOfDayFailedException);
	}

	return time;
}

Date Date_GetCurrent(void) {
	Date_UnixTime time = Date_GetCurrentUnixTime();
	return Date_FromUnixEpoch(time.sec);
}
