#import "Integer.h"

typedef struct {
	uint16_t year;
	uint8_t  month;
	uint8_t  day;
} Date;

typedef enum {
	Date_Month_Unset = 0,
	Date_Month_January,
	Date_Month_Februray,
	Date_Month_March,
	Date_Month_April,
	Date_Month_May,
	Date_Month_June,
	Date_Month_July,
	Date_Month_August,
	Date_Month_September,
	Date_Month_October,
	Date_Month_November,
	Date_Month_December
} Date_Month;

typedef enum {
	Date_WeekDay_Sunday = 0,
	Date_WeekDay_Monday,
	Date_WeekDay_Tuesday,
	Date_WeekDay_Wednesday,
	Date_WeekDay_Thursday,
	Date_WeekDay_Friday,
	Date_WeekDay_Saturday,
	Date_WeekDay_Unset
} Date_WeekDay;

#define Date_SecondsMinute (60)
#define Date_SecondsHour   (Date_SecondsMinute *  60)
#define Date_SecondsDay    (Date_SecondsHour   *  24)
#define Date_SecondsYear   (Date_SecondsDay    * 365)

const short Date_DaysPerMonth[13];

void Date_Init(Date *this);
bool Date_IsLeapYear(int year);
short Date_Compare(Date a, Date b);
bool Date_Equals(Date a, Date b);
