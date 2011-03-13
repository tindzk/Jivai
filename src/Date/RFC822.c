#import "RFC822.h"

#define self Date_RFC822

static const RdString pattern = $("^(\\S+), (\\d+) (\\S+) (\\d+) (\\d+):(\\d+):(\\d+)");

static const RdString weekdays[] = {
	$("Sun"),
	$("Mon"),
	$("Tue"),
	$("Wed"),
	$("Thu"),
	$("Fri"),
	$("Sat")
};

static const RdString months[] = {
	$(""),
	$("Jan"),
	$("Feb"),
	$("Mar"),
	$("Apr"),
	$("May"),
	$("Jun"),
	$("Jul"),
	$("Aug"),
	$("Sep"),
	$("Oct"),
	$("Nov"),
	$("Dec")
};

sdef(self, Empty) {
	self res;

	res.date = Date_Empty();
	res.time = Time_Empty();

	res.weekday = Date_WeekDay_Unset;

	return res;
}

sdef(self, Parse, RdString s) {
	RdString weekday, day, month, year, hour, minute, second;

	Pattern regex;
	Pattern_Init(&regex);
	Pattern_Compile(&regex, pattern);

	self res;

	if (!Pattern_Match(&regex, s, Pattern_Result(NULL, &weekday, &day, &month, &year, &hour, &minute, &second))) {
		res.date    = DateTime_GetUnixEpoch().date;
		res.time    = DateTime_GetUnixEpoch().time;
		res.weekday = Date_WeekDay_Unset;

		goto out;
	}

	res.date.day     = Int8_Parse(day);
	res.date.month   = Date_Month_Unset;
	res.date.year    = Int16_Parse(year);
	res.time.hour    = Int8_Parse(hour);
	res.time.minute  = Int8_Parse(minute);
	res.time.second  = Int8_Parse(second);
	res.weekday      = Date_WeekDay_Unset;

	for (u8 i = 0; i < 7; i++) {
		if (String_Equals(weekdays[i], weekday)) {
			res.weekday = i;
			break;
		}
	}

	for (u8 i = 1; i <= 12; i++) {
		if (String_Equals(months[i], month)) {
			res.date.month = i;
			break;
		}
	}

out:
	Pattern_Destroy(&regex);

	return res;
}

sdef(self, FromDate, Date $this) {
	self res;

	res.date    = $this;
	res.time    = Time_Empty();
	res.weekday = Date_GetWeekDay($this);

	return res;
}

sdef(self, FromDateTime, DateTime $this) {
	self res;

	res.date    = $this.date;
	res.time    = $this.time;
	res.weekday = Date_GetWeekDay($this.date);

	return res;
}

sdef(String, ToString, self $this) {
	if ($this.date.month > 12) {
		$this.date.month = 0;
	}

	String day    = Number_Format($this.date.day,    2);
	String year   = Number_Format($this.date.year,   4);
	String hour   = Number_Format($this.time.hour,   2);
	String minute = Number_Format($this.time.minute, 2);
	String second = Number_Format($this.time.second, 2);

	String out = String_Format($("%, % % % %:%:% GMT"),
		weekdays[$this.weekday], day.rd,
		months[$this.date.month], year.rd, hour.rd,
		minute.rd, second.rd);

	String_Destroy(&second);
	String_Destroy(&minute);
	String_Destroy(&hour);
	String_Destroy(&year);
	String_Destroy(&day);

	return out;
}
