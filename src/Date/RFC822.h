#import "../Date.h"
#import "../Time.h"
#import "../Number.h"
#import "../String.h"
#import "../Pattern.h"
#import "../DateTime.h"

typedef struct {
	Date date;
	Time time;

	Date_WeekDay weekday;
} Date_RFC822;

void Date_RFC822_Init(Date_RFC822 *this);
Date_RFC822 Date_RFC822_Parse(String s);
Date_RFC822 Date_RFC822_FromDate(DateTime date);
String Date_RFC822_ToString(Date_RFC822 date);
