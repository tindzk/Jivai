#import <Float.h>
#import <String.h>

#import "TestSuite.h"
#import "App.h"

#undef self
#define self tsFloat

class(self) {

};

tsRegister("Float") {
	return true;
}

tsCase(Acute, "Format") {
	String s = Float_ToString(12., 0.01);
	Assert($("No decimal places"), String_Equals(s, $("12.00")));

	s = Float_ToString(1234.98765, 0.001);
	Assert($("Trim decimal places"), String_Equals(s, $("1234.987")));

	s = Float_ToString(.98765, 0.001);
	Assert($("Leading zero (< 1)"), String_Equals(s, $("0.987")));
}

tsFinalize;
