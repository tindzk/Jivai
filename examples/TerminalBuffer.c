#include <Terminal/Buffer.h>
#include <ExceptionManager.h>

ExceptionManager exc;

int main(void) {
	ExceptionManager_Init(&exc);

	Terminal0(&exc);

	Terminal term;
	Terminal_Init(&term, File_StdIn, File_StdOut, true);
	Terminal_Configure(&term, false, false);

	Terminal_Buffer termbuf;
	Terminal_Buffer_Init(&termbuf, &term, 1);

	int first = Terminal_Buffer_NewChunk(&termbuf);

	Terminal_Buffer_SetAttr(&termbuf,
		Terminal_Color_ForegroundRed,
		Terminal_Font_Italics);

	Terminal_Buffer_Print(&termbuf,
		String("Red foreground and italic font."));

	int second = Terminal_Buffer_NewChunk(&termbuf);

	Terminal_Buffer_SetAttr(&termbuf,
		Terminal_Color_Normal,
		Terminal_Font_Normal);

	Terminal_Buffer_Print(&termbuf,
		String("Normal text."));

	int third = Terminal_Buffer_NewChunk(&termbuf);

	Terminal_Buffer_SetAttr(&termbuf,
		Terminal_Color_Normal,
		Terminal_Font_Bold);

	Terminal_Buffer_Print(&termbuf,
		String("Bold text."));

	sleep(1);

	Terminal_Buffer_ChangeAttr(&termbuf,
		second,
		Terminal_Color_ForegroundGreen,
		Terminal_Font_Bold);

	sleep(1);

	Terminal_Buffer_ChangeValue(&termbuf,
		first,
		String("New value."));

	sleep(1);

	Terminal_Buffer_ChangeAttr(&termbuf,
		third,
		Terminal_Color_BackgroundBlue | Terminal_Color_ForegroundWhite,
		Terminal_Font_Normal);

	Terminal_Buffer_ChangeValue(&termbuf,
		third,
		String("Blue text."));

	sleep(3);

	Terminal_Buffer_Destroy(&termbuf);
	Terminal_Destroy(&term);

	return EXIT_SUCCESS;
}
