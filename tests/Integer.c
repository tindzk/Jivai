#import <String.h>
#import <Integer.h>

#import "TestSuite.h"

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
	bool caught = false;

	try {
		UInt32_Parse($("-1"));
	} catch(Integer, Underflow) {
		caught = true;
	} finally {

	} tryEnd;

	Assert($("Detect underflow"), caught);
}

tsCase(Acute, "Parsing UInt32") {
	u32 val = 0;
	bool caught = false;

	try {
		val = UInt32_Parse($("4294967295"));
	} catch(Integer, Overflow) {
		caught = true;
	} finally {

	} tryEnd;

	Assert($("No overflow"), !caught);
	Assert($("Correct number"), val == 4294967295);
}

tsCase(Acute, "Parsing UInt32") {
	bool caught = false;

	try {
		UInt32_Parse($("4294967296"));
	} catch(Integer, Overflow) {
		caught = true;
	} finally {

	} tryEnd;

	Assert($("Detect overflow"), caught);
}

tsCase(Acute, "Parsing Int16") {
	bool caught = false;

	try {
		Int16_Parse($("1114111"));
	} catch(Integer, Overflow) {
		caught = true;
	} finally {

	} tryEnd;

	Assert($("Detect overflow"), caught);
}

tsCase(Acute, "Parsing UInt32") {
	bool caught = false;

	try {
		UInt32_Parse($("-42949672960"));
	} catch(Integer, Underflow) {
		caught = true;
	} finally {

	} tryEnd;

	Assert($("Detect underflow"), caught);
}

tsCase(Acute, "Parsing UInt32") {
	bool caught = false;

	try {
		UInt32_Parse($("42949672960"));
	} catch(Integer, Overflow) {
		caught = true;
	} finally {

	} tryEnd;

	Assert($("Detect overflow"), caught);
}

tsCase(Acute, "Parsing UInt64") {
	u64 val = 0;
	bool caught = false;

	try {
		val = UInt64_Parse($("18446744073709551615"));
	} catch(Integer, Overflow) {
		caught = true;
	} finally {

	} tryEnd;

	Assert($("No overflow"), !caught);
	Assert($("Correct value"), val == 18446744073709551615);
}

tsCase(Acute, "Parsing UInt64") {
	bool caught = false;

	try {
		UInt64_Parse($("18446744073709551616"));
	} catch(Integer, Overflow) {
		caught = true;
	} finally {

	} tryEnd;

	Assert($("Detect overflow"), caught);
}

tsCase(Acute, "Parsing UInt64") {
	bool caught = false;

	try {
		UInt64_Parse($("184467440737095516150"));
	} catch(Integer, Overflow) {
		caught = true;
	} finally {

	} tryEnd;

	Assert($("Detect overflow"), caught);
}

tsCase(Acute, "Parsing Int64") {
	bool caught = false;

	try {
		Int64_Parse($("-184467440737095516150"));
	} catch(Integer, Underflow) {
		caught = true;
	} finally {

	} tryEnd;

	Assert($("Detect underflow"), caught);
}

tsCase(Acute, "Parsing Int64") {
	bool caught = false;

	try {
		Int64_Parse($("184467440737095516150"));
	} catch(Integer, Overflow) {
		caught = true;
	} finally {

	} tryEnd;

	Assert($("Detect overflow"), caught);
}

tsCase(Acute, "Int32 conversion") {
	String s = Int32_ToString(-123456789);
	Assert($("Negative numbers"), String_Equals(s.rd, $("-123456789")));
	String_Destroy(&s);

	s = Int32_ToString(123456789);
	Assert($("Positive numbers"), String_Equals(s.rd, $("123456789")));
	String_Destroy(&s);
}

tsCase(Acute, "Int64 conversion") {
	String s = Int64_ToString(-12345678987654321);
	Assert($("Negative numbers"), String_Equals(s.rd, $("-12345678987654321")));
	String_Destroy(&s);

	s = Int64_ToString(12345678987654321);
	Assert($("Positive numbers"), String_Equals(s.rd, $("12345678987654321")));
	String_Destroy(&s);
}
