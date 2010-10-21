#import <Terminal/InputLine.h>

ExceptionManager exc;

typedef struct {
	bool interrupt;
} Context;

void OnKeyEnter(__unused Context *this, String s) {
	String_Print(String("\nBuffer contains: '"));
	String_Print(s);
	String_Print(String("'\n"));
}

bool OnKeyPress(Context *this, String ch) {
	if (ch.buf[0] == CTRLKEY('c')) {
		String_Print(String("Shutting down...\n"));
		this->interrupt = true;
		return true;
	} else if (ch.buf[0] == CTRLKEY('q')) {
		String_Print(String("You pressed CTRL-Q!\n"));
		return true;
	}

	return false;
}

int main(void) {
	ExceptionManager_Init(&exc);

	String0(&exc);
	Terminal_InputLine0(&exc);

	Terminal term;
	Terminal_Init(&term, File_StdIn, File_StdOut, true);
	Terminal_Configure(&term, false, false);

	Terminal_InputLine line;
	Terminal_InputLine_Init(&line, &term);

	Context context;
	context.interrupt = false;

	line.context = &context;

	line.onKeyPress = (void *) &OnKeyPress;
	line.onKeyEnter = (void *) &OnKeyEnter;

	int res = ExitStatus_Success;

	try (&exc) {
		while (!context.interrupt) {
			Terminal_InputLine_Process(&line);
		}
	} clean catchAny {
		ExceptionManager_Print(&exc, e);

#if Exception_SaveTrace
		Backtrace_PrintTrace(exc.e.trace, exc.e.traceItems);
#endif

		res = ExitStatus_Failure;
	} finally {
		Terminal_InputLine_Destroy(&line);
		Terminal_Destroy(&term);
	} tryEnd;

	return res;
}
