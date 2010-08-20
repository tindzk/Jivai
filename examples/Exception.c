#include <String.h>

ExceptionManager exc;

Exception_Define(CustomException);

void func3(void) {
	throw(&exc, &CustomException);
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
	} catch(&CustomException, e) {
		String_Print(String("CustomException caught.\n"));

#if Exception_SaveTrace
		Backtrace_PrintTrace(e->trace, e->traceItems);
#endif
	} finally {
		String_Print(String("finally block.\n"));
	} tryEnd;

	return EXIT_SUCCESS;
}
