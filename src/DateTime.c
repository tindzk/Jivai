#import "DateTime.h"

Exception_Define(YearLower1970Exception);

static ExceptionManager *exc;

void DateTime0(ExceptionManager *e) {
	exc = e;
}

void DateTime_Init(DateTime *this) {
	Date_Init(&this->date);
	Time_Init(&this->time);
}

short DateTime_Compare(DateTime a, DateTime b) {
	short date = Date_Compare(a.date, b.date);

	if (date != 0) {
		return date;
	}

	return Time_Compare(a.time, b.time);
}

inline bool DateTime_Equals(DateTime a, DateTime b) {
	return Date_Equals(a.date, b.date)
		&& Time_Equals(a.time, b.time);
}

DateTime DateTime_FromUnixEpoch(uint64_t time) {
	DateTime res;

	res.date.year = (int)(time / Date_SecondsYear);
	time -= (int)(res.date.year * Date_SecondsYear);

	res.date.day = (int)(time / Date_SecondsDay);
	time -= (int)(res.date.day * Date_SecondsDay);

	res.time.hour = (int)(time / Date_SecondsHour);
	time -= (int)(res.time.hour * Date_SecondsHour);

	res.time.minute = (int)(time / Date_SecondsMinute);
	time -= (int)(res.time.minute * Date_SecondsMinute);

	res.time.second = time;

	/* Subtract one day for each leap year. */
	res.date.day -= (res.date.year + 1) / 4;

	/* The day has already begun. */
	res.date.day++;

	res.date.month = 0;

	for (size_t i = 12; i >= 1; i--) {
		if (res.date.day / Date_DaysPerMonth[i]) {
			res.date.day -= Date_DaysPerMonth[i];
			res.date.month = i;

			break;
		}
	}

	res.date.month++;

	res.date.year += 1970;

	return res;
}

uint64_t DateTime_ToUnixEpoch(DateTime *this) {
	if (this->date.year < 1970) {
		throw(exc, &YearLower1970Exception);
	}

	short years = this->date.year - 1970;

	/* Days since 1970. */
	int days = years * 365;

	/* Add one day for each leap year since 1970. */
	days += (years + 1) / 4;

	/* Add one extra day when the year is a leap year and the January is already over. */
	if (this->date.month > 1) {
		if (Date_IsLeapYear(this->date.year)) {
			days++;
		}
	}

	/* Add number of days up until the end of the current month. */
	days += Date_DaysPerMonth[this->date.month - 1] + this->date.day - 1;

	uint64_t hours   = days    * 24 + this->time.hour;
	uint64_t minutes = hours   * 60 + this->time.minute;
	uint64_t seconds = minutes * 60 + this->time.second;

	return seconds;
}

DateTime DateTime_GetCurrent(void) {
	Time_UnixEpoch time = Time_GetCurrentUnixTime();
	return DateTime_FromUnixEpoch(time.sec);
}