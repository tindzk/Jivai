#import <Float.h>
#import <String.h>

#import "TestSuite.h"

#define self tsFloat

class {

};

tsRegister("Float") {
	return true;
}

tsCase(Acute, "Format") {
	String s = Float_ToString(0., 0.01, '.');
	Assert($("No decimal places"), String_Equals(s, $("0.00")));
	String_Destroy(&s);

	s = Float_ToString(200., 0.01, '.');
	Assert($("No decimal places (2)"), String_Equals(s, $("200.00")));
	String_Destroy(&s);

	s = Float_ToString(12., 0.01, '.');
	Assert($("No decimal places (3)"), String_Equals(s, $("12.00")));
	String_Destroy(&s);

	s = Float_ToString(1234.98765, 0.001, '.');
	Assert($("Trim decimal places"), String_Equals(s, $("1234.987")));
	String_Destroy(&s);

	s = Float_ToString(.98765, 0.001, '.');
	Assert($("Leading zero (< 1)"), String_Equals(s, $("0.987")));
	String_Destroy(&s);
}

tsCase(Acute, "Multiplication") {
	float num = 12.;
	String s = Float_ToString(num * 1.13, 0.01, '.');
	Assert($("12.00 * 1.13"), String_Equals(s, $("13.56")));
	String_Destroy(&s);

	num = 0.3333;
	s = Float_ToString(num * 1.13, 0.01, '.');
	Assert($(".3333 * 1.13"), String_Equals(s, $("0.37")));
	String_Destroy(&s);
}

tsCase(Acute, "Parsing") {
	float number = Float_Parse($("12.00"), '.');
	Assert($("12.00"), Float_Equals(number, 12., 0.00001));

	number = Float_Parse($("12.45"), '.');
	Assert($("12.45"), Float_Equals(number, 12.45, 0.00001));

	number = Float_Parse($("-12.00"), '.');
	Assert($("-12.00"), Float_Equals(number, -12., 0.00001));

	number = Float_Parse($("-12.45"), '.');
	Assert($("-12.45"), Float_Equals(number, -12.45, 0.00001));

	number = Float_Parse($("3"), '.');
	Assert($("3"), Float_Equals(number, 3., 0.00001));

	number = Float_Parse($("-3"), '.');
	Assert($("-3"), Float_Equals(number, -3, 0.00001));
}

tsFinalize;
