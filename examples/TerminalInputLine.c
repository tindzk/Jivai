#import <Terminal/InputLine.h>

#define self App

class {
	bool interrupt;
};

def(void, OnKeyEnter, RdString s) {
	String_Print($("\nBuffer contains: '"));
	String_Print(s);
	String_Print($("'\n"));
}

def(bool, OnKeyPress, RdString ch) {
	if (ch.buf[0] == CTRLKEY('c')) {
		String_Print($("Shutting down...\n"));
		this->interrupt = true;
		return true;
	} else if (ch.buf[0] == CTRLKEY('q')) {
		String_Print($("You pressed CTRL-Q!\n"));
		return true;
	}

	return false;
}

#undef self

int main(void) {
	Terminal term = Terminal_New(true);
	Terminal_Configure(&term, false, false);

	Terminal_InputLine line = Terminal_InputLine_New(&term);

	App app;
	app.interrupt = false;

	line.onKeyPress = Terminal_InputLine_OnKeyPress_For(&app, App_OnKeyPress);
	line.onKeyEnter = Terminal_InputLine_OnKeyEnter_For(&app, App_OnKeyEnter);

	int res = ExitStatus_Success;

	try {
		while (!app.interrupt) {
			Terminal_InputLine_Process(&line);
		}
	} catchAny {
		Exception_Print(e);
		res = ExitStatus_Failure;
	} finally {
		Terminal_InputLine_Destroy(&line);
		Terminal_Destroy(&term);
	} tryEnd;

	return res;
}
