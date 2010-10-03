#import <String.h>

ExceptionManager exc;

#undef self
#define self Example

size_t Modules_Example;

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

	Modules_Example = Module_Register(String("Example"));

	try (&exc) {
		func1();
	} catch(Modules_Example, excCustomException, e) {
		String_Print(String("CustomException caught.\n"));

#if Exception_SaveTrace
		Backtrace_PrintTrace(e->trace, e->traceItems);
#endif
	} finally {
		String_Print(String("finally block.\n"));
	} tryEnd;

	return EXIT_SUCCESS;
}
