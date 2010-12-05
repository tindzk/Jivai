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
	Assert($("Hour"),   dt.time.hour   == 12);
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
	Assert($("Day"),    dt.date.day    == 13);
	Assert($("Hour"),   dt.time.hour   == 23);
	Assert($("Minute"), dt.time.minute == 31);
	Assert($("Second"), dt.time.second == 30);
}

tsCase(Acute, "Unix epoch conversion (4)") {
	DateTime dt = {
		.date.year   = 2010,
		.date.month  = 12,
		.date.day    = 5,
		.time.hour   = 12,
		.time.minute = 57,
		.time.second = 37
	};

	Assert($("1291553857"), DateTime_ToUnixEpoch(dt) == 1291553857);
}

tsCase(Acute, "Unix epoch conversion (5)") {
	DateTime dt = {
		.date.year   = 2010,
		.date.month  = 1,
		.date.day    = 1,
		.time.hour   = 0,
		.time.minute = 0,
		.time.second = 0
	};

	Assert($("1262304000"), DateTime_ToUnixEpoch(dt) == 1262304000);
}

tsCase(Acute, "Unix epoch conversion (6)") {
	DateTime dt = {
		.date.year   = 2009,
		.date.month  = 2,
		.date.day    = 13,
		.time.hour   = 23,
		.time.minute = 31,
		.time.second = 30
	};

	Assert($("1234567890"), DateTime_ToUnixEpoch(dt) == 1234567890);
}

tsCase(Acute, "Unix epoch conversion (7)") {
	DateTime dt = {
		.date.year   = 2010,
		.date.month  = 12,
		.date.day    = 30,
		.time.hour   = 0,
		.time.minute = 0,
		.time.second = 0
	};

	Assert($("1293667200"), DateTime_ToUnixEpoch(dt) == 1293667200);
}

tsCase(Acute, "Drift (+0)") {
	DateTime dt = {
		.date.year   = 2009,
		.date.month  = 2,
		.date.day    = 13,
		.time.hour   = 23,
		.time.minute = 31,
		.time.second = 30
	};

	DateTime dtNew = DateTime_Drift(dt, 0, 0);

	Assert($("Equals"), DateTime_Compare(dt, dtNew) == 0);
}

tsCase(Acute, "Drift (+0)") {
	DateTime dt = {
		.date.year   = 2010,
		.date.month  = 12,
		.date.day    = 30,
		.time.hour   = 0,
		.time.minute = 0,
		.time.second = 0
	};

	DateTime dtNew = DateTime_Drift(dt, 0, 0);

	Assert($("Equals"), DateTime_Compare(dt, dtNew) == 0);
}

tsCase(Acute, "Drift (+1)") {
	DateTime dt = {
		.date.year   = 2010,
		.date.month  = 12,
		.date.day    = 30,
		.time.hour   = 0,
		.time.minute = 0,
		.time.second = 0
	};

	DateTime dtNew = DateTime_Drift(dt, 1, 0);

	Assert($("Hour"),   dt.time.hour + 1 == dtNew.time.hour);
	Assert($("Minute"), dt.time.minute   == dtNew.time.minute);
}

tsFinalize;
