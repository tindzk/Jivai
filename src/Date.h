#import "String.h"
#import "Integer.h"

#define self Date

typedef u16 Year;
typedef u16 YearDay;
typedef u16 YearWeek;
typedef u8  YearMonth;
typedef u8  MonthDay;
typedef u8  WeekDay;

class {
	Year      year;
	YearMonth month;
	MonthDay  day;
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

record(ref(Week)) {
	Year     year;
	YearWeek week;
};

enum {
	ref(SecondsMinute) = 60,
	ref(SecondsHour)   = ref(SecondsMinute) *  60,
	ref(SecondsDay)    = ref(SecondsHour)   *  24,
	ref(SecondsWeek)   = ref(SecondsDay)    *   7,
	ref(SecondsMonth)  = ref(SecondsDay)    *  30,
	ref(SecondsYear)   = ref(SecondsDay)    * 365
};

const YearMonth ref(DaysPerMonth)[13];
const YearDay   ref(AddedDaysPerMonth)[13];
const RdString  ref(MonthNames)[13];
const RdString  ref(WeekDays)[8];

rsdef(self, New);
rsdef(bool, IsLeapYear, Year year);
rsdef(YearWeek, GetWeekNumber, self date);
rsdef(ref(Week), getRealWeek, self date);
rsdef(self, fromWeek, ref(Week) week);
rsdef(short, Compare, self a, self b);
rsdef(YearDay, getDayOfYear, self date);
rsdef(self, fromDayOfYear, Year year, YearDay day);
rsdef(WeekDay, GetWeekDay, self date);
rsdef(String, Format, self date, bool wday);

static alwaysInline rsdef(bool, Equals, self a, self b) {
	return scall(Compare, a, b) == 0;
}

#undef self
