#import "../Date.h"
#import "../Time.h"
#import "../Number.h"
#import "../String.h"
#import "../Pattern.h"
#import "../DateTime.h"

#define self Date_RFC822

record(self) {
	Date date;
	Time time;

	Date_WeekDay weekday;
};

sdef(self, Empty);
sdef(self, Parse, ProtString s);
sdef(self, FromDate, Date $this);
sdef(self, FromDateTime, DateTime $this);
sdef(String, ToString, self $this);

#undef self
