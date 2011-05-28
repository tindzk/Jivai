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
}

tsCase(Acute, "Week day for non-leap years") {
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
	Assert($("2011-01-01"),
		Date_GetRealWeekNumber((Date) {2011, 1, 1}) == 1);

	Assert($("2011-01-03"),
		Date_GetRealWeekNumber((Date) {2011, 1, 3}) == 2);

	Assert($("2011-01-10"),
		Date_GetRealWeekNumber((Date) {2011, 1, 10}) == 3);

	Assert($("2011-01-11"),
		Date_GetRealWeekNumber((Date) {2011, 1, 11}) == 3);

	Assert($("2011-01-12"),
		Date_GetRealWeekNumber((Date) {2011, 1, 12}) == 3);

	Assert($("2011-01-15"),
		Date_GetRealWeekNumber((Date) {2011, 1, 15}) == 3);

	Assert($("2011-01-16"),
		Date_GetRealWeekNumber((Date) {2011, 1, 16}) == 3);

	Assert($("2011-01-17"),
		Date_GetRealWeekNumber((Date) {2011, 1, 17}) == 4);
}
