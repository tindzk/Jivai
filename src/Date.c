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

const RdString ref(MonthNames)[] = {
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

const RdString ref(WeekDays)[] = {
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

/*
 * Returns the week number as ISO 8601.
 *
 * Rendered after McCarty's algorithm:
 * http://personal.ecu.edu/mccartyr/ISOwdALG.txt
 */

sdef(size_t, GetWeekNumber, self date) {
	size_t dayOfYear   = scall(GetDayOfYear, date);
	size_t jan1WeekDay = scall(GetWeekDay, (Date) { date.year, 1, 1 });
	size_t weekDay     = scall(GetWeekDay, date);
	size_t weekNumber  = 0;
	size_t yearNumber  = date.year;

	if (dayOfYear <= 8 - jan1WeekDay && jan1WeekDay > 4) {
		yearNumber--;

		if (jan1WeekDay == 5 || (jan1WeekDay == 6 && scall(IsLeapYear, date.year - 1))) {
			weekNumber = 53;
		} else {
			weekNumber = 52;
		}
	}

	if (yearNumber == date.year) {
		size_t totalDays;

		if (scall(IsLeapYear, date.year)) {
			totalDays = 366;
		} else {
			totalDays = 365;
		}

		if (totalDays - dayOfYear < 4 - weekDay) {
			yearNumber++;
			weekNumber = 1;
		}
	}

	if (yearNumber == date.year) {
		size_t j = dayOfYear + (7 - weekDay) + (jan1WeekDay - 1);

		weekNumber = j / 7;

		if (jan1WeekDay > 4) {
			weekNumber--;
		}
	}

	return weekNumber;
}

sdef(short, GetRealWeekNumber, self date) {
	size_t dayOfYear   = scall(GetDayOfYear, date);
	size_t jan1WeekDay = scall(GetWeekDay, (Date) { date.year, 1, 1 });

	return (7 + (dayOfYear - 1) + (jan1WeekDay - 1)) / 7;
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

sdef(size_t, GetDayOfYear, self date) {
	int days = date.day;

	/* Add number of days up until last month. */
	days += ref(AddedDaysPerMonth)[date.month - 1];

	/* Add one extra day when the year is a leap year and the
	 * February is already over.
	 */
	if (scall(IsLeapYear, date.year)) {
		if (date.month > 2) {
			days++;
		}
	}

	return days;
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
	RdString month = ref(MonthNames)[date.month];

	String day  = Integer_ToString(date.day);
	String year = Integer_ToString(date.year);

	RdString suffix;

	if (date.day == 1) {
		suffix = $("st");
	} else if (date.day == 2) {
		suffix = $("nd");
	} else if (date.day == 3) {
		suffix = $("rd");
	} else {
		suffix = $("th");
	}

	String res;

	if (wday) {
		res = String_Format($("%, % %%, %"),
			ref(WeekDays)[scall(GetWeekDay, date)],
			month, day.rd, suffix, year.rd);
	} else {
		res = String_Format($("% %%, %"),
			month, day.rd, suffix, year.rd);
	}

	String_Destroy(&year);
	String_Destroy(&day);

	return res;
}
