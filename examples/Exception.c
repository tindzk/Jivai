#import <String.h>
#import <Exception.h>

#define self Example

//@excCustomException

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
		String_Print(String("CustomException caught.\n"));

#if Exception_SaveTrace
		Backtrace_PrintTrace(__exc_mgr.e.trace, __exc_mgr.e.traceItems);
#endif
	} finally {
		String_Print(String("finally block.\n"));
	} tryEnd;

	return ExitStatus_Success;
}
