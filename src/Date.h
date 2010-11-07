#import "Integer.h"

#undef self
#define self Date

record(self) {
	uint16_t year;
	uint8_t  month;
	uint8_t  day;
};

set(ref(Month)) {
	ref(Month_Unset) = 0,
	ref(Month_January),
	ref(Month_Februray),
	ref(Month_March),
	ref(Month_April),
	ref(Month_May),
	ref(Month_June),
	ref(Month_July),
	ref(Month_August),
	ref(Month_September),
	ref(Month_October),
	ref(Month_November),
	ref(Month_December)
};

set(ref(WeekDay)) {
	ref(WeekDay_Sunday) = 0,
	ref(WeekDay_Monday),
	ref(WeekDay_Tuesday),
	ref(WeekDay_Wednesday),
	ref(WeekDay_Thursday),
	ref(WeekDay_Friday),
	ref(WeekDay_Saturday),
	ref(WeekDay_Unset)
};

enum {
	ref(SecondsMinute) = 60,
	ref(SecondsHour)   = ref(SecondsMinute) *  60,
	ref(SecondsDay)    = ref(SecondsHour)   *  24,
	ref(SecondsYear)   = ref(SecondsDay)    * 365
};

const short ref(DaysPerMonth)[13];

sdef(self, Empty);
sdef(bool, IsLeapYear, int year);
sdef(short, Compare, self a, self b);
sdef(bool, Equals, self a, self b);
sdef(short, GetWeekDay, self date);
