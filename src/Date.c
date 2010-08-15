#include "Date.h"

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

void Date_Init(Date *this) {
	this->year   = 0;
	this->month  = 0;
	this->day    = 0;
}

/* Taken from diet libc (dietlibc-0.32/libugly/isleap.c) */
bool Date_IsLeapYear(int year) {
	/* Every fourth year is a leap year except for century years that are
	 * not divisible by 400.
	 */
	return (!(year % 4) && ((year % 100) || !(year % 400)));
}

short Date_Compare(Date a, Date b) {
	short year = Integer_Compare(a.year, b.year);

	if (year != 0) {
		return year;
	}

	short month = Integer_Compare(a.month, b.month);

	if (month != 0) {
		return month;
	}

	return Integer_Compare(a.day, b.day);
}

inline bool Date_Equals(Date a, Date b) {
	return Date_Compare(a, b) == 0;
}
