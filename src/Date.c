#import "Date.h"

#define self Date

const short ref(DaysPerMonth)[] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

/* Contains the days up until the end of a month. Refers to a non-leap year.
 * Taken from diet libc (dietlibc-0.32/libugly/time_table_spd.c).
 */
const short ref(AddedDaysPerMonth)[] = {
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

const String ref(MonthNames)[] = {
	$(""),
	$("January"),
	$("Februray"),
	$("March"),
	$("April"),
	$("May"),
	$("June"),
	$("July"),
	$("August"),
	$("September"),
	$("October"),
	$("November"),
	$("December")
};

const String ref(WeekDays)[] = {
	$("Sunday"),
	$("Monday"),
	$("Tuesday"),
	$("Wednesday"),
	$("Thursday"),
	$("Friday"),
	$("Saturday"),
	$("")
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
	short year = Int16_Compare(a.year, b.year);

	if (year != 0) {
		return year;
	}

	short month = Int16_Compare(a.month, b.month);

	if (month != 0) {
		return month;
	}

	return Int16_Compare(a.day, b.day);
}

inline sdef(bool, Equals, self a, self b) {
	return scall(Compare, a, b) == 0;
}

/* See also DateTime_ToUnixEpoch(). */
sdef(short, GetWeekDay, self date) {
	short year = date.year - 1900;

	int day = (year * 365) + (year / 4);

	day += ref(AddedDaysPerMonth)[date.month - 1] + date.day;

	if (date.month < 2) {
		if (scall(IsLeapYear, date.year)) {
			day--;
		}
	}

	return (short) (day % 7);
}

sdef(String, Format, self date, bool wday) {
	String month = ref(MonthNames)[date.month];

	String day  = Int32_ToString(date.day);
	String year = Int32_ToString(date.year);

	String suffix;

	if (date.day == 1) {
		suffix = $("st");
	} else if (date.day == 2) {
		suffix = $("nd");
	} else if (date.day == 3) {
		suffix = $("rd");
	} else {
		suffix = $("th");
	}

	if (wday) {
		return String_Format($("%, % %%, %"),
			ref(WeekDays)[scall(GetWeekDay, date)],
			month, day, suffix, year);
	}

	return String_Format($("% %%, %"),
		month, day, suffix, year);
}
