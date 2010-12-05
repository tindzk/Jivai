#import <String.h>
#import <DateTime.h>

#import "TestSuite.h"

#undef self
#define self tsDateTime

class {

};

tsRegister("DateTime") {
	return true;
}

tsCase(Acute, "Unix epoch conversion") {
	DateTime dt = DateTime_FromUnixEpoch(1291553857);

	Assert($("Year"),   dt.date.year   == 2010);
	Assert($("Month"),  dt.date.month  == 12);
	Assert($("Day"),    dt.date.day    == 5);
	Assert($("Hour"),   dt.time.hour   == 13);
	Assert($("Minute"), dt.time.minute == 57);
	Assert($("Second"), dt.time.second == 37);
}

tsCase(Acute, "Unix epoch conversion (2)") {
	DateTime dt = DateTime_FromUnixEpoch(1262304000);

	Assert($("Year"),   dt.date.year   == 2010);
	Assert($("Month"),  dt.date.month  == 1);
	Assert($("Day"),    dt.date.day    == 1);
	Assert($("Hour"),   dt.time.hour   == 0);
	Assert($("Minute"), dt.time.minute == 0);
	Assert($("Second"), dt.time.second == 0);
}

tsCase(Acute, "Unix epoch conversion (3)") {
	DateTime dt = DateTime_FromUnixEpoch(1234567890);

	Assert($("Year"),   dt.date.year   == 2009);
	Assert($("Month"),  dt.date.month  == 2);
	Assert($("Day"),    dt.date.day    == 14);
	Assert($("Hour"),   dt.time.hour   == 0);
	Assert($("Minute"), dt.time.minute == 31);
	Assert($("Second"), dt.time.second == 30);
}

tsFinalize;
