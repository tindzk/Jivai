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

void Date_RFC822_Init(Date_RFC822 *this) {
	Date_Init(&this->date);
	Time_Init(&this->time);

	this->weekday = Date_WeekDay_Unset;
}

Date_RFC822 Date_RFC822_Parse(String s) {
	String weekday, day, month, year, hour, minute, second;

	Pattern regex;
	Pattern_Init(&regex);
	Pattern_Compile(&regex, pattern);

	Date_RFC822 res;

	if (!Pattern_Match(&regex, s, Pattern_Result(NULL, &weekday, &day, &month, &year, &hour, &minute, &second))) {
		res.date    = DateTime_UnixEpoch.date;
		res.time    = DateTime_UnixEpoch.time;
		res.weekday = Date_WeekDay_Unset;

		goto out;
	}

	res.date.day     = (uint8_t) Integer_ParseString(day);
	res.date.month   = Date_Month_Unset;
	res.date.year    = (uint16_t) Integer_ParseString(year);
	res.time.hour    = (uint8_t) Integer_ParseString(hour);
	res.time.minute  = (uint8_t) Integer_ParseString(minute);
	res.time.second  = (uint8_t) Integer_ParseString(second);
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

Date_RFC822 Date_RFC822_FromDate(DateTime this) {
	Date_RFC822 res;

	res.date    = this.date;
	res.time    = this.time;
	res.weekday = Date_GetWeekDay(this.date);

	return res;
}

String Date_RFC822_ToString(Date_RFC822 this) {
	if (this.date.month > 12) {
		this.date.month = 0;
	}

	String tmp1, tmp2, tmp3, tmp4, tmp5;

	String out = String_Format(String("%, % % % %:%:% GMT"),
		weekdays[this.weekday],
		tmp1 = Number_Format(this.date.day,    2),
		months[this.date.month],
		tmp2 = Number_Format(this.date.year,   4),
		tmp3 = Number_Format(this.time.hour,   2),
		tmp4 = Number_Format(this.time.minute, 2),
		tmp5 = Number_Format(this.time.second, 2));

	String_Destroy(&tmp5);
	String_Destroy(&tmp4);
	String_Destroy(&tmp3);
	String_Destroy(&tmp2);
	String_Destroy(&tmp1);

	return out;
}
