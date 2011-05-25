#import <Main.h>
#import <String.h>
#import <Exception.h>

#define self Application

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

def(bool, Run) {
	try {
		func1();
	} catch(Application, CustomException) {
		String_Print($("CustomException caught.\n"));
	} finally {
		String_Print($("finally block.\n"));
	} tryEnd;

	return true;
}
