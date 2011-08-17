#import "Date.h"

#define self Date

const YearMonth ref(DaysPerMonth)[] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

/* Contains the days up until the end of a month. Refers to a non-leap year.
 * Taken from diet libc (dietlibc-0.32/libugly/time_table_spd.c).
 */
const YearDay ref(AddedDaysPerMonth)[] = {
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

rsdef(self, New) {
	return (self) {
		.year  = 0,
		.month = 0,
		.day   = 0
	};
}

/* Taken from diet libc (dietlibc-0.32/libugly/isleap.c) */
rsdef(bool, IsLeapYear, Year year) {
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

rsdef(YearWeek, GetWeekNumber, self date) {
	YearDay  dayOfYear   = scall(GetDayOfYear, date);
	YearDay  jan1WeekDay = scall(GetWeekDay, (self) { date.year, 1, 1 });
	WeekDay  weekDay     = scall(GetWeekDay, date);
	YearWeek weekNumber  = 0;
	Year     yearNumber  = date.year;

	if (dayOfYear <= 8 - jan1WeekDay && jan1WeekDay > 4) {
		yearNumber--;

		if (jan1WeekDay == 5 || (jan1WeekDay == 6 && scall(IsLeapYear, date.year - 1))) {
			weekNumber = 53;
		} else {
			weekNumber = 52;
		}
	}

	if (yearNumber == date.year) {
		YearDay totalDays;

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
		uint j = dayOfYear + (7 - weekDay) + (jan1WeekDay - 1);

		weekNumber = j / 7;

		if (jan1WeekDay > 4) {
			weekNumber--;
		}
	}

	return weekNumber;
}

rsdef(ref(Week), getRealWeek, self date) {
	YearDay dayOfYear   = scall(GetDayOfYear, date);
	YearDay jan1WeekDay = scall(GetWeekDay, (self) { date.year, 1, 1 });

	/* The last day of this year's first week. */
	YearDay lastDay = 7 - jan1WeekDay + 1;

	if (date.month > 1 || date.day > lastDay) {
		/* The date belongs to this year. */
		return (ref(Week)) {
			.year = date.year,
			.week = (dayOfYear - 1 + jan1WeekDay - 1) / 7
		};
	} else {
		return (ref(Week)) {
			.year = date.year - 1,
			.week = 52
		};
	}
}

rsdef(short, Compare, self a, self b) {
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

rsdef(YearDay, GetDayOfYear, self date) {
	YearDay days = date.day;

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
rsdef(WeekDay, GetWeekDay, self date) {
	Year year = date.year - 1900;

	uint day = (year * 365) + (year / 4);

	day += ref(AddedDaysPerMonth)[date.month - 1] + date.day;

	if (date.month < 2) {
		if (scall(IsLeapYear, date.year)) {
			day--;
		}
	}

	return day % 7;
}

rsdef(String, Format, self date, bool wday) {
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
