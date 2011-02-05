#import "Integer.h"

#define self Date

record(self) {
	u16 year;
	u8  month;
	u8  day;
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
	ref(SecondsWeek)   = ref(SecondsDay)    *   7,
	ref(SecondsYear)   = ref(SecondsDay)    * 365
};

const short ref(DaysPerMonth)[13];
const short ref(AddedDaysPerMonth)[13];
const String ref(MonthNames)[13];
const String ref(WeekDays)[8];

sdef(self, Empty);
sdef(bool, IsLeapYear, int year);
sdef(size_t, GetWeekNumber, self date);
sdef(short, Compare, self a, self b);
sdef(bool, Equals, self a, self b);
sdef(size_t, GetDayOfYear, self date);
sdef(short, GetWeekDay, self date);
sdef(String, Format, self date, bool wday);

#undef self
