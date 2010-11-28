#import <Terminal/InputLine.h>

#undef self
#define self App

class {
	bool interrupt;
};

def(void, OnKeyEnter, String s) {
	String_Print(String("\nBuffer contains: '"));
	String_Print(s);
	String_Print(String("'\n"));
}

def(bool, OnKeyPress, String ch) {
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

#undef self

int main(void) {
	Terminal term;
	Terminal_Init(&term, File_StdIn, File_StdOut, true);
	Terminal_Configure(&term, false, false);

	Terminal_InputLine line;
	Terminal_InputLine_Init(&line, &term);

	App app;
	app.interrupt = false;

	line.onKeyPress = (Terminal_InputLine_OnKeyPress) Callback(&app, App_OnKeyPress);
	line.onKeyEnter = (Terminal_InputLine_OnKeyEnter) Callback(&app, App_OnKeyEnter);

	int res = ExitStatus_Success;

	try {
		while (!app.interrupt) {
			Terminal_InputLine_Process(&line);
		}
	} clean catchAny {
		Exception_Print(e);

#if Exception_SaveTrace
		Backtrace_PrintTrace(__exc_mgr.e.trace, __exc_mgr.e.traceItems);
#endif

		res = ExitStatus_Failure;
	} finally {
		Terminal_InputLine_Destroy(&line);
		Terminal_Destroy(&term);
	} tryEnd;

	return res;
}
