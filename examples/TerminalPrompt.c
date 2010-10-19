#import <Terminal/Prompt.h>

ExceptionManager exc;

int main(void) {
	ExceptionManager_Init(&exc);

	Terminal0(&exc);

	Terminal term;
	Terminal_Init(&term, File_StdIn, File_StdOut, true);
	Terminal_Configure(&term, false, false);

	Terminal_Prompt prompt;
	Terminal_Prompt_Init(&prompt, &term);

	bool isYes = Terminal_Prompt_Ask(&prompt, String("Hello world?"));

	Terminal_Prompt_Destroy(&prompt);

	if (isYes) {
		Terminal_Print(&term,
			Terminal_Color_ForegroundRed,
			Terminal_Font_Normal,
			String("'Yes' selected.\n"));
	} else {
		Terminal_Print(&term,
			Terminal_Color_ForegroundGreen,
			Terminal_Font_Normal,
			String("'No' selected.\n"));
	}

	Terminal_Destroy(&term);

	return ExitStatus_Success;
}
