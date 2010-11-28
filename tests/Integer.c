#import <String.h>
#import <Integer.h>

#import "TestSuite.h"

extern ExceptionManager exc;

#undef self
#define self tsInteger

class {

};

tsRegister("Integer") {
	return true;
}

tsCase(Acute, "Parsing Int32") {
	s32 num = Int32_Parse($("-123456789"));
	Assert($("Negative numbers"), num == -123456789);

	num = Int32_Parse($("123456789"));
	Assert($("Positive numbers"), num == 123456789);

	num = Int32_Parse($("1"));
	Assert($("Positive number"), num == 1);

	num = Int32_Parse($("0"));
	Assert($("Empty string"), num == 0);
}

tsCase(Acute, "Parsing UInt32") {
	bool ok = false;

	try {
		UInt32_Parse($("-1"));
	} clean catch(Integer, excUnsignedType) {
		ok = true;
	} finally {

	} tryEnd;

	Assert($("Negative numbers"), ok);
}

tsCase(Acute, "Int32 conversion") {
	String s = Int32_ToString(-123456789);
	Assert($("Negative numbers"), String_Equals(s, $("-123456789")));

	s = Int32_ToString(123456789);
	Assert($("Positive numbers"), String_Equals(s, $("123456789")));
}

tsCase(Acute, "Int64 conversion") {
	String s = Int64_ToString(-12345678987654321);
	Assert($("Negative numbers"), String_Equals(s, $("-12345678987654321")));

	s = Int64_ToString(12345678987654321);
	Assert($("Positive numbers"), String_Equals(s, $("12345678987654321")));
}

tsFinalize;
