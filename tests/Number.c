#import <String.h>
#import <Number.h>
#import <Integer.h>

#import "TestSuite.h"

#define self tsNumber

class {

};

tsRegister("Number") {
	return true;
}

tsCase(Acute, "Format") {
	String s = Number_Format(12345678, 2);
	Assert($("12345678"), String_Equals(s.rd, $("12345678")));
	String_Destroy(&s);

	s = Number_Format(0, 2);
	Assert($("0"), String_Equals(s.rd, $("00")));
	String_Destroy(&s);

	s = Number_Format(1, 5);
	Assert($("1"), String_Equals(s.rd, $("00001")));
	String_Destroy(&s);

	s = Number_Format(-12345678, 2);
	Assert($("-123456789"), String_Equals(s.rd, $("-12345678")));
	String_Destroy(&s);

	s = Number_Format(-123, 2);
	Assert($("-123"), String_Equals(s.rd, $("-123")));
	String_Destroy(&s);

	s = Number_Format(-1, 2);
	Assert($("-1"), String_Equals(s.rd, $("-01")));
	String_Destroy(&s);
}

tsFinalize;
