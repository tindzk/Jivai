#import "RFC822.h"

static const String pattern = String("^(\\S+), (\\d+) (\\S+) (\\d+) (\\d+):(\\d+):(\\d+)");

static const String weekdays[] = {
	String("Sun"),
	String("Mon"),
	String("Tue"),
	String("Wed"),
	String("Thu"),
	String("Fri"),
	String("Sat")
};

static const String months[] = {
	String(""),
	String("Jan"),
	String("Feb"),
	String("Mar"),
	String("Apr"),
	String("May"),
	String("Jun"),
	String("Jul"),
	String("Aug"),
	String("Sep"),
	String("Oct"),
	String("Nov"),
	String("Dec")
};

sdef(self, Empty) {
	self res;

	res.date = Date_Empty();
	res.time = Time_Empty();

	res.weekday = Date_WeekDay_Unset;

	return res;
}

sdef(self, Parse, String s) {
	String weekday, day, month, year, hour, minute, second;

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

	for (uint8_t i = 0; i < 7; i++) {
		if (String_Equals(weekdays[i], weekday)) {
			res.weekday = i;
			break;
		}
	}

	for (uint8_t i = 1; i <= 12; i++) {
		if (String_Equals(months[i], month)) {
			res.date.month = i;
			break;
		}
	}

out:
	Pattern_Destroy(&regex);

	return res;
}

sdef(self, FromDate, DateTime $this) {
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

	String tmp1, tmp2, tmp3, tmp4, tmp5;

	String out = String_Format(String("%, % % % %:%:% GMT"),
		weekdays[$this.weekday],
		tmp1 = Number_Format($this.date.day,    2),
		months[$this.date.month],
		tmp2 = Number_Format($this.date.year,   4),
		tmp3 = Number_Format($this.time.hour,   2),
		tmp4 = Number_Format($this.time.minute, 2),
		tmp5 = Number_Format($this.time.second, 2));

	String_Destroy(&tmp5);
	String_Destroy(&tmp4);
	String_Destroy(&tmp3);
	String_Destroy(&tmp2);
	String_Destroy(&tmp1);

	return out;
}
