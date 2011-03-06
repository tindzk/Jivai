#import <Main.h>
#import <String.h>
#import <Exception.h>

#define self Example

// @exc CustomException

void func3(void) {
	throw(CustomException);
}

void func2(void) {
	func3();
}

void func1(void) {
	func2();
}

bool Main(__unused ProtString base, __unused ProtStringArray *args) {
	try {
		func1();
	} clean catch(Example, CustomException) {
		String_Print($("CustomException caught.\n"));
	} finally {
		String_Print($("finally block.\n"));
	} tryEnd;

	return true;
}
