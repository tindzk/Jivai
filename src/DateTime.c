#import "DateTime.h"

sdef(self, Empty) {
	self res;

	res.date = Date_Empty();
	res.time = Time_Empty();

	return res;
}

sdef(self, FromDate, Date date) {
	self res;

	res.date = date;
	res.time = Time_Empty();

	return res;
}

inline sdef(self, GetUnixEpoch) {
	return (DateTime) {
		.date = {
			.year  = 1970,
			.month = 1,
			.day   = 1
		},

		.time = {
			.hour   = 0,
			.minute = 0,
			.second = 0
		}
	};
}

sdef(short, Compare, self a, self b) {
	short date = Date_Compare(a.date, b.date);

	if (date != 0) {
		return date;
	}

	return Time_Compare(a.time, b.time);
}

sdef(self, Drift, self dateTime, s8 hrs, s8 mins) {
	u64 secs = scall(ToUnixEpoch, dateTime);

	secs += hrs  * Date_SecondsHour;
	secs += mins * Date_SecondsMinute;

	return scall(FromUnixEpoch, secs);
}

inline sdef(bool, Equals, self a, self b) {
	return Date_Equals(a.date, b.date)
		&& Time_Equals(a.time, b.time);
}

sdef(self, FromUnixEpoch, u64 time) {
	self res;

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

sdef(u64, ToUnixEpoch, self dateTime) {
	if (dateTime.date.year < 1970) {
		throw(excYearLower1970);
	}

	short years = dateTime.date.year - 1970;

	/* Days since 1970. */
	int days = years * 365;

	/* Add one day for each leap year since 1970. */
	days += (years + 1) / 4;

	/* Add one extra day when the year is a leap year and the January is already over. */
	if (dateTime.date.month > 1) {
		if (Date_IsLeapYear(dateTime.date.year)) {
			days++;
		}
	}

	/* Add number of days up until the end of the current month. */
	days += Date_DaysPerMonth[dateTime.date.month - 1] + dateTime.date.day - 1;

	u64 hours   = days    * 24 + dateTime.time.hour;
	u64 minutes = hours   * 60 + dateTime.time.minute;
	u64 seconds = minutes * 60 + dateTime.time.second;

	return seconds;
}

sdef(self, GetCurrent) {
	Time_UnixEpoch time = Time_GetCurrentUnixTime();
	return scall(FromUnixEpoch, time.sec);
}
