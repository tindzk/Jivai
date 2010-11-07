#import "Date.h"

/* Contains the days up until the end of a month. Refers to a non-leap year.
 * Taken from diet libc (dietlibc-0.32/libugly/time_table_spd.c).
 */
const short ref(DaysPerMonth)[13] = {
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

sdef(self, Empty) {
	self res;

	res.year  = 0;
	res.month = 0;
	res.day   = 0;

	return res;
}

/* Taken from diet libc (dietlibc-0.32/libugly/isleap.c) */
sdef(bool, IsLeapYear, int year) {
	/* Every fourth year is a leap year except for century years that are
	 * not divisible by 400.
	 */
	return (!(year % 4) && ((year % 100) || !(year % 400)));
}

sdef(short, Compare, self a, self b) {
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

inline sdef(bool, Equals, self a, self b) {
	return scall(Compare, a, b) == 0;
}

/* See also DateTime_ToUnixEpoch(). */
sdef(short, GetWeekDay, self date) {
	short year = date.year - 1900;

	int day = (year * 365) + (year / 4);

	day += ref(DaysPerMonth)[date.month - 1] + date.day;

	if (date.month < 2) {
		if (scall(IsLeapYear, date.year)) {
			day--;
		}
	}

	return (short) (day % 7);
}
