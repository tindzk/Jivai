#import <Terminal/Selection.h>

int main(void) {
	Terminal term;
	Terminal_Init(&term, File_StdIn, File_StdOut, true);
	Terminal_Configure(&term, false, false);

	Terminal_Selection termsel;
	Terminal_Selection_Init(&termsel, &term);

	String arr[5];

	Terminal_Selection_Add(&termsel, arr[0] = String("First"),  true);
	Terminal_Selection_Add(&termsel, arr[1] = String("Second"), false);
	Terminal_Selection_Add(&termsel, arr[2] = String("Third"),  false);
	Terminal_Selection_Add(&termsel, arr[3] = String("Fourth"), false);
	Terminal_Selection_Add(&termsel, arr[4] = String("Fifth"),  false);

	ssize_t sel = Terminal_Selection_GetSel(&termsel);

	Terminal_Selection_Destroy(&termsel);

	if (sel < 0) {
		String_Print(String("Aborted."));
	} else {
		String_Print(String("You selected: "));
		String_Print(arr[sel]);
	}

	String_Print(String("\n"));

	Terminal_Destroy(&term);

	return ExitStatus_Success;
}
