#import <Terminal/Prompt.h>

int main(void) {
	Terminal term;
	Terminal_Init(&term, File_StdIn, File_StdOut, true);
	Terminal_Configure(&term, false, false);

	Terminal_Prompt prompt;
	Terminal_Prompt_Init(&prompt, &term);

	bool isYes = Terminal_Prompt_Ask(&prompt, $("Hello world?"));

	Terminal_Prompt_Destroy(&prompt);

	if (isYes) {
		Terminal_Print(&term,
			Terminal_Color_ForegroundRed,
			Terminal_Font_Normal,
			$("'Yes' selected.\n"));
	} else {
		Terminal_Print(&term,
			Terminal_Color_ForegroundGreen,
			Terminal_Font_Normal,
			$("'No' selected.\n"));
	}

	Terminal_Destroy(&term);

	return ExitStatus_Success;
}
