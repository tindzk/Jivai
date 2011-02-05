#import "DateTime.h"

#define self DateTime

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

	res.date.year = time / Date_SecondsYear;
	time -= res.date.year * Date_SecondsYear;

	s32 days = (s32) time / Date_SecondsDay;
	time -= days * Date_SecondsDay;

	res.time.hour = time / Date_SecondsHour;
	time -= res.time.hour * Date_SecondsHour;

	res.time.minute = time / Date_SecondsMinute;
	time -= res.time.minute * Date_SecondsMinute;

	res.time.second = time;

	days++;

	/* See ToUnixEpoch(). */
	days -= (res.date.year + 1) / 4;

	if (days <= 0) {
		days += 365;
		res.date.year--;
	}

	res.date.month = 0;

	reverse (i, 12) {
		if (days > Date_AddedDaysPerMonth[i]) {
			days -= Date_AddedDaysPerMonth[i];
			res.date.month = i;
			break;
		}
	}

	res.date.month++;

	res.date.day   = days;
	res.date.year += 1970;

	/* See ToUnixEpoch(). */
	if (Date_IsLeapYear(res.date.year)) {
		if (res.date.month > 2) {
			res.date.day--;
		}
	}

	return res;
}

sdef(u64, ToUnixEpoch, self dateTime) {
	if (dateTime.date.year < 1970) {
		throw(YearLower1970);
	}

	short years = dateTime.date.year - 1970;

	int days = Date_GetDayOfYear(dateTime.date);

	/* Add days since 1970. */
	days += years * 365;

	/* Add one day for each leap year since 1970. */
	days += (years + 1) / 4;

	days--;

	u64 hours   = days    * 24 + dateTime.time.hour;
	u64 minutes = hours   * 60 + dateTime.time.minute;
	u64 seconds = minutes * 60 + dateTime.time.second;

	return seconds;
}

sdef(self, GetCurrent) {
	Time_UnixEpoch time = Time_GetCurrent();
	return scall(FromUnixEpoch, time.sec);
}

sdef(String, Format, self dt) {
	String date = Date_Format(dt.date, true);
	String time = Time_Format(dt.time);

	String fmt = String_Format($("% %"), date, time);

	String_Destroy(&time);
	String_Destroy(&date);

	return fmt;
}
