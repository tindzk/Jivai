#import <Terminal/Selection.h>

int main(void) {
	Terminal term;
	Terminal_Init(&term, File_StdIn, File_StdOut, true);
	Terminal_Configure(&term, false, false);

	Terminal_Selection termsel;
	Terminal_Selection_Init(&termsel, &term);

	String arr[5];

	Terminal_Selection_Add(&termsel, arr[0] = $("First"),  true);
	Terminal_Selection_Add(&termsel, arr[1] = $("Second"), false);
	Terminal_Selection_Add(&termsel, arr[2] = $("Third"),  false);
	Terminal_Selection_Add(&termsel, arr[3] = $("Fourth"), false);
	Terminal_Selection_Add(&termsel, arr[4] = $("Fifth"),  false);

	ssize_t sel = Terminal_Selection_GetSel(&termsel);

	Terminal_Selection_Destroy(&termsel);

	if (sel < 0) {
		String_Print($("Aborted."));
	} else {
		String_Print($("You selected: "));
		String_Print(arr[sel]);
	}

	String_Print($("\n"));

	Terminal_Destroy(&term);

	return ExitStatus_Success;
}
