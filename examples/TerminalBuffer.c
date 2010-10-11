#import <Terminal/Buffer.h>

ExceptionManager exc;

int main(void) {
	ExceptionManager_Init(&exc);

	Terminal0(&exc);

	Terminal term;
	Terminal_Init(&term, File_StdIn, File_StdOut, true);
	Terminal_Configure(&term, false, false);

	Terminal_Buffer termbuf;
	Terminal_Buffer_Init(&termbuf, &term, 1);

	Terminal_Buffer_Chunk chunk;
	chunk.color = Terminal_Color_ForegroundRed;
	chunk.font  = Terminal_Font_Italics;
	chunk.value = String_Clone(String("Red foreground and italic font."));

	size_t first = Terminal_Buffer_AddChunk(&termbuf, chunk);

	chunk.color = Terminal_Color_Normal;
	chunk.font  = Terminal_Font_Normal;
	chunk.value = String_Clone(String("Normal text."));

	size_t second = Terminal_Buffer_AddChunk(&termbuf, chunk);

	chunk.color = Terminal_Color_Normal;
	chunk.font  = Terminal_Font_Bold;
	chunk.value = String_Clone(String("Bold text."));

	size_t third = Terminal_Buffer_AddChunk(&termbuf, chunk);

	sleep(1);

	Terminal_Buffer_ChangeAttr(&termbuf,
		second,
		Terminal_Color_ForegroundGreen,
		Terminal_Font_Bold);

	Terminal_Buffer_Redraw(&termbuf);

	sleep(1);

	Terminal_Buffer_ChangeValue(&termbuf,
		first,
		String("New value."));

	Terminal_Buffer_Redraw(&termbuf);

	sleep(1);

	Terminal_Buffer_ChangeAttr(&termbuf,
		third,
		Terminal_Color_BackgroundBlue | Terminal_Color_ForegroundWhite,
		Terminal_Font_Normal);

	Terminal_Buffer_ChangeValue(&termbuf,
		third,
		String("Blue text."));

	Terminal_Buffer_Redraw(&termbuf);

	sleep(3);

	Terminal_Buffer_Destroy(&termbuf);
	Terminal_Destroy(&term);

	return EXIT_SUCCESS;
}
