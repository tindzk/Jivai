#import "Buffer.h"

typedef struct {
	Terminal *term;
	Terminal_Buffer termbuf;
} Terminal_Prompt;

void Terminal_Prompt_Init(Terminal_Prompt *this, Terminal *term);
bool Terminal_Prompt_Ask(Terminal_Prompt *this, String msg);
void Terminal_Prompt_Destroy(Terminal_Prompt *this);
