#import <Date.h>
#import <Time.h>
#import <Date/RFC822.h>

#import "TestSuite.h"

#define self tsDate_RFC822

class {

};

tsRegister("Date.RFC822") {
	return true;
}

tsCase(Acute, "Week day for leap years") {
	Date_RFC822 rfc822 = Date_RFC822_Parse($("Tue, 19 Apr 2011 19:10:07 +0200"));

	Date date = { .year = 2011, .month  = 4, .day = 19  };
	Time time = { .hour = 19, .minute = 10, .second = 7 };

	Assert($("Date"),    Date_Equals(rfc822.date, date));
	Assert($("Time"),    Time_Equals(rfc822.time, time));
	Assert($("Weekday"), rfc822.weekday == 2);
}

tsFinalize;
