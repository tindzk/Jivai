#include "Prompt.h"

void Terminal_Prompt_Init(Terminal_Prompt *this, Terminal *term) {
	this->term = term;
	Terminal_Buffer_Init(&this->termbuf, term, 1);
}

void Terminal_Prompt_Destroy(Terminal_Prompt *this) {
	Terminal_Buffer_Clear(&this->termbuf);
	Terminal_Buffer_Destroy(&this->termbuf);
}

bool Terminal_Prompt_Ask(Terminal_Prompt *this, String msg) {
	Terminal_Buffer_NewChunk(&this->termbuf);
	Terminal_Buffer_Print(&this->termbuf, msg);

	Terminal_Buffer_NewChunk(&this->termbuf);
	Terminal_Buffer_Print(&this->termbuf, String("[y/n]"));

	/* Add a trailing space. */
	Terminal_Buffer_NewChunk(&this->termbuf);

	while (true) {
		Terminal_Key key = Terminal_ReadKey(this->term);

		if (key.c == 'y') {
			return true;
		} else if (key.c == 'n') {
			return false;
		}
	}
}
