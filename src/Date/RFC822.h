#import "../Date.h"
#import "../Time.h"
#import "../Number.h"
#import "../String.h"
#import "../Pattern.h"
#import "../DateTime.h"

#define self Date_RFC822

class {
	Date date;
	Time time;

	Date_WeekDay weekday;
};

rsdef(self, New);
rsdef(self, Parse, RdString s);
rsdef(self, FromDate, Date date);
rsdef(self, FromDateTime, DateTime dt);
rsdef(String, ToString, self $this);

#undef self
