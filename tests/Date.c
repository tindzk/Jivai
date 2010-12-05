#import <Date.h>
#import <String.h>

#import "TestSuite.h"

#undef self
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

tsFinalize;
