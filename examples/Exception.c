#import <String.h>
#import <Exception.h>

ExceptionManager exc;

#undef self
#define self Example

enum {
	excCustomException = excOffset
};

void func3(void) {
	throw(&exc, excCustomException);
}

void func2(void) {
	func3();
}

void func1(void) {
	func2();
}

int main(void) {
	ExceptionManager_Init(&exc);

	String0(&exc);
	Memory0(&exc);

	try (&exc) {
		func1();
	} clean catch(Example, excCustomException) {
		String_Print(String("CustomException caught.\n"));

#if Exception_SaveTrace
		Backtrace_PrintTrace(exc.e.trace, exc.e.traceItems);
#endif
	} finally {
		String_Print(String("finally block.\n"));
	} tryEnd;

	return ExitStatus_Success;
}
