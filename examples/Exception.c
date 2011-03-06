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

int main(void) {
	try {
		func1();
	} clean catch(Example, CustomException) {
		String_Print($("CustomException caught.\n"));

#if Exception_SaveTrace
		Backtrace_PrintTrace(
			Exception_GetTraceBuffer(),
			Exception_GetTraceLength());
#endif
	} finally {
		String_Print($("finally block.\n"));
	} tryEnd;

	return ExitStatus_Success;
}
