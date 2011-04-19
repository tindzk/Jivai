#import "Integer.h"

#define self Date

class {
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
const RdString ref(MonthNames)[13];
const RdString ref(WeekDays)[8];

rsdef(self, New);
rsdef(bool, IsLeapYear, int year);
rsdef(size_t, GetWeekNumber, self date);
rsdef(short, GetRealWeekNumber, self date);
rsdef(short, Compare, self a, self b);
rsdef(size_t, GetDayOfYear, self date);
rsdef(short, GetWeekDay, self date);
rsdef(String, Format, self date, bool wday);

static alwaysInline rsdef(bool, Equals, self a, self b) {
	return scall(Compare, a, b) == 0;
}

#undef self
