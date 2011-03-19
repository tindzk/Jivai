#import <Terminal.h>

int main(void) {
	Terminal term = Terminal_New(true);
	Terminal_Configure(&term, false, false);

	Terminal_Print(&term,
		Terminal_Color_Normal,
		Terminal_Font_Normal,
		$("Normal."));

	Terminal_Print(&term,
		Terminal_Color_Normal,
		Terminal_Font_Normal,
		$("\n"));

	Terminal_Print(&term,
		Terminal_Color_Normal,
		Terminal_Font_Bold,
		$("Bold font.\n"));

	Terminal_Print(&term,
		Terminal_Color_Normal,
		Terminal_Font_Italics,
		$("Italic font.\n"));

	Terminal_Print(&term,
		Terminal_Color_Normal,
		Terminal_Font_Underline,
		$("Underlined font.\n"));

	Terminal_Print(&term,
		Terminal_Color_Normal,
		Terminal_Font_Blink,
		$("Blinking font.\n"));

	Terminal_Print(&term,
		Terminal_Color_ForegroundRed,
		Terminal_Font_Normal,
		$("Red foreground.\n"));

	Terminal_Print(&term,
		Terminal_Color_BackgroundRed,
		Terminal_Font_Normal,
		$("Red background."));

	Terminal_Print(&term,
		Terminal_Color_Normal,
		Terminal_Font_Normal,
		$("\n"));

	Terminal_Print(&term,
		Terminal_Color_ForegroundRed,
		Terminal_Font_Bold | Terminal_Font_Italics,
		$("Red foreground, bold and italic font.\n"));

	Terminal_Destroy(&term);

	return ExitStatus_Success;
}
