#include <Terminal.h>
#include <ExceptionManager.h>

ExceptionManager exc;

int main(void) {
	ExceptionManager_Init(&exc);

	Terminal0(&exc);

	Terminal term;
	Terminal_Init(&term, File_StdIn, File_StdOut, true);
	Terminal_Configure(&term, false, false);

	Terminal_Print(&term,
		Terminal_Color_Normal,
		Terminal_Font_Normal,
		String("Normal."));

	Terminal_Print(&term,
		Terminal_Color_Normal,
		Terminal_Font_Normal,
		String("\n"));

	Terminal_Print(&term,
		Terminal_Color_Normal,
		Terminal_Font_Bold,
		String("Bold font.\n"));

	Terminal_Print(&term,
		Terminal_Color_Normal,
		Terminal_Font_Italics,
		String("Italic font.\n"));

	Terminal_Print(&term,
		Terminal_Color_Normal,
		Terminal_Font_Underline,
		String("Underlined font.\n"));

	Terminal_Print(&term,
		Terminal_Color_Normal,
		Terminal_Font_Blink,
		String("Blinking font.\n"));

	Terminal_Print(&term,
		Terminal_Color_ForegroundRed,
		Terminal_Font_Normal,
		String("Red foreground.\n"));

	Terminal_Print(&term,
		Terminal_Color_BackgroundRed,
		Terminal_Font_Normal,
		String("Red background."));

	Terminal_Print(&term,
		Terminal_Color_Normal,
		Terminal_Font_Normal,
		String("\n"));

	Terminal_Print(&term,
		Terminal_Color_ForegroundRed,
		Terminal_Font_Bold | Terminal_Font_Italics,
		String("Red foreground, bold and italic font.\n"));

	Terminal_Destroy(&term);

	return EXIT_SUCCESS;
}
