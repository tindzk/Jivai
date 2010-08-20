#include <Terminal/InputLine.h>
#include <ExceptionManager.h>

ExceptionManager exc;

typedef struct {
	bool interrupt;
} Context;

void OnKeyEnter(UNUSED Context *this, String s) {
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

#include <stdio.h>

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

	int res = EXIT_SUCCESS;

	try(&exc) {
		while (!context.interrupt) {
			Terminal_InputLine_Process(&line);
		}
	} catchAny(e) {
		Exception_Print(e);

#if Exception_SaveTrace
		Backtrace_PrintTrace(e->trace, e->traceItems);
#endif

		res = EXIT_FAILURE;
	} finally {
		Terminal_InputLine_Destroy(&line);
		Terminal_Destroy(&term);
	} tryEnd;

	return res;
}
