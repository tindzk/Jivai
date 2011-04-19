#import "RFC822.h"

#define self Date_RFC822

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

rsdef(self, New) {
	return (self) {
		.date    = Date_New(),
		.time    = Time_New(),
		.weekday = Date_WeekDay_Unset
	};
}

rsdef(self, Parse, RdString s) {
	self res;

	RdString pattern = $("%, % % % %:%:% ");
	RdString weekday, day, month, year, hour, minute, second;

	if (!String_Parse(pattern, s, &weekday, &day, &month, &year, &hour, &minute, &second)) {
		return scall(New);
	}

	try {
		res.date.day     = UInt8_Parse(day);
		res.date.month   = Date_Month_Unset;
		res.date.year    = UInt16_Parse(year);
		res.time.hour    = UInt8_Parse(hour);
		res.time.minute  = UInt8_Parse(minute);
		res.time.second  = UInt8_Parse(second);
		res.weekday      = Date_WeekDay_Unset;
	} catchModule(Integer) {
		excReturn scall(New);
	} finally {

	} tryEnd;

	fwd(i, nElems(weekdays)) {
		if (String_Equals(weekdays[i], weekday)) {
			res.weekday = i;
			break;
		}
	}

	fwd(i, nElems(months)) {
		if (String_Equals(months[i], month)) {
			res.date.month = i;
			break;
		}
	}

	return res;
}

rsdef(self, FromDate, Date date) {
	return (self) {
		.date    = date,
		.time    = Time_New(),
		.weekday = Date_GetWeekDay(date)
	};
}

rsdef(self, FromDateTime, DateTime dt) {
	return (self) {
		.date    = dt.date,
		.time    = dt.time,
		.weekday = Date_GetWeekDay(dt.date)
	};
}

rsdef(String, ToString, self $this) {
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
