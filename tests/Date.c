#import <Date.h>
#import <String.h>

#import "TestSuite.h"

#define self tsDate

class {

};

tsRegister("Date") {
	return true;
}

tsCase(Acute, "Week day for leap years") {
	Assert($("1996-05-15"),
		Date_GetWeekDay((Date) {1996, 5, 15}) == 3);

	Assert($("1996-01-15"),
		Date_GetWeekDay((Date) {1996, 1, 15}) == 1);

	Assert($("2012-01-01"),
		Date_GetWeekDay((Date) {2012, 1, 1}) == 0);
}

tsCase(Acute, "Week day for non-leap years") {
	Assert($("2011-12-31"),
		Date_GetWeekDay((Date) {2011, 12, 31}) == 6);

	Assert($("2010-10-03"),
		Date_GetWeekDay((Date) {2010, 10, 3}) == 0);

	Assert($("2010-10-02"),
		Date_GetWeekDay((Date) {2010, 10, 2}) == 6);

	Assert($("2010-10-01"),
		Date_GetWeekDay((Date) {2010, 10, 1}) == 5);

	Assert($("2010-10-01"),
		Date_GetWeekDay((Date) {2010, 10, 1}) == 5);

	Assert($("1997-05-15"),
		Date_GetWeekDay((Date) {1997, 5, 15}) == 4);

	Assert($("1997-01-15"),
		Date_GetWeekDay((Date) {1997, 1, 15}) == 3);
}

tsCase(Acute, "Week number") {
	Assert($("2007-01-01"),
		Date_GetWeekNumber((Date) {2007, 1, 1}) == 1);

	Assert($("2008-01-01"),
		Date_GetWeekNumber((Date) {2008, 1, 1}) == 1);

	Assert($("2009-12-31"),
		Date_GetWeekNumber((Date) {2009, 12, 31}) == 53);

	Assert($("2010-01-01"),
		Date_GetWeekNumber((Date) {2010, 1, 1}) == 53);
}

tsCase(Acute, "Real week number") {
	Date_Week week = Date_getRealWeek((Date) {2011, 1, 1});
	Assert($("2011-01-01"), week.year == 2010 && week.week == 52);

	week = Date_getRealWeek((Date) {2011, 1, 2});
	Assert($("2011-01-02"), week.year == 2010 && week.week == 52);

	week = Date_getRealWeek((Date) {2011, 1, 3});
	Assert($("2011-01-03"), week.year == 2011 && week.week == 1);

	week = Date_getRealWeek((Date) {2011, 1, 10});
	Assert($("2011-01-10"), week.year == 2011 && week.week == 2);

	week = Date_getRealWeek((Date) {2011, 1, 11});
	Assert($("2011-01-11"), week.year == 2011 && week.week == 2);

	week = Date_getRealWeek((Date) {2011, 1, 15});
	Assert($("2011-01-15"), week.year == 2011 && week.week == 2);

	week = Date_getRealWeek((Date) {2011, 1, 16});
	Assert($("2011-01-16"), week.year == 2011 && week.week == 2);

	week = Date_getRealWeek((Date) {2011, 1, 17});
	Assert($("2011-01-17"), week.year == 2011 && week.week == 3);

	week = Date_getRealWeek((Date) {2011, 12, 31});
	Assert($("2011-12-31"), week.year == 2011 && week.week == 52);

	week = Date_getRealWeek((Date) {2012, 12, 31});
	Assert($("2012-12-31"), week.year == 2012 && week.week == 52);
}

tsCase(Acute, "Real week number") {
	Date date = Date_fromWeek((Date_Week) { 2010, 52 });
	Assert($("2010-12-27"), date.year == 2010 && date.month == 12 && date.day == 27);

	date = Date_fromWeek((Date_Week) { 2011, 1 });
	Assert($("2011-01-03"), date.year == 2011 && date.month == 1 && date.day == 3);

	date = Date_fromWeek((Date_Week) { 2011, 2 });
	Assert($("2011-01-10"), date.year == 2011 && date.month == 1 && date.day == 10);

	date = Date_fromWeek((Date_Week) { 2011, 3 });
	Assert($("2011-01-17"), date.year == 2011 && date.month == 1 && date.day == 17);
}

tsCase(Acute, "Date to day of year") {
	Assert($("2011-01-01"), Date_getDayOfYear((Date) { 2011, 1, 1  }) == 1);
	Assert($("2011-01-31"), Date_getDayOfYear((Date) { 2011, 1, 31 }) == 31);
	Assert($("2011-02-01"), Date_getDayOfYear((Date) { 2011, 2, 1  }) == 31 + 1);
	Assert($("2011-03-01"), Date_getDayOfYear((Date) { 2011, 3, 1  }) == 31 + 28 + 1);

	Assert($("2012-02-01"), Date_getDayOfYear((Date) { 2012, 2, 1  }) == 31 + 1);
	Assert($("2012-02-29"), Date_getDayOfYear((Date) { 2012, 2, 29 }) == 31 + 29);
	Assert($("2012-03-01"), Date_getDayOfYear((Date) { 2012, 3, 1  }) == 31 + 29 + 1);
}

tsCase(Acute, "Date from day of year") {
	Date date = Date_fromDayOfYear(2011, 1);
	Assert($("2011-01-01"),
		date.year == 2011 && date.month == 1 && date.day == 1);

	date = Date_fromDayOfYear(2011, 31);
	Assert($("2011-01-31"),
		date.year == 2011 && date.month == 1 && date.day == 31);

	date = Date_fromDayOfYear(2011, 31 + 1);
	Assert($("2011-02-01"),
		date.year == 2011 && date.month == 2 && date.day == 1);

	date = Date_fromDayOfYear(2012, 31 + 1);
	Assert($("2012-02-01"),
		date.year == 2012 && date.month == 2 && date.day == 1);

	date = Date_fromDayOfYear(2012, 31 + 29);
	Assert($("2012-02-29"),
		date.year == 2012 && date.month == 2 && date.day == 29);

	date = Date_fromDayOfYear(2012, 31 + 29 + 1);
	Assert($("2012-03-01"),
		date.year == 2012 && date.month == 3 && date.day == 1);
}
