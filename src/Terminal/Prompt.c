#import "Prompt.h"

void Terminal_Prompt_Init(Terminal_Prompt *this, Terminal *term) {
	this->term = term;
	Terminal_Buffer_Init(&this->termbuf, term, 1);
}

void Terminal_Prompt_Destroy(Terminal_Prompt *this) {
	Terminal_Buffer_Destroy(&this->termbuf);
}

bool Terminal_Prompt_Ask(Terminal_Prompt *this, String msg) {
	Terminal_Buffer_Chunk chunk;

	chunk.color = Terminal_Color_Normal;
	chunk.font  = Terminal_Font_Normal;
	chunk.value = String_Clone(msg);

	Terminal_Buffer_AddChunk(&this->termbuf, chunk);

	chunk.value = String_Clone(String("[y/n] "));

	Terminal_Buffer_AddChunk(&this->termbuf, chunk);

	while (true) {
		Terminal_Key key = Terminal_ReadKey(this->term);

		if (key.c == 'y') {
			return true;
		} else if (key.c == 'n') {
			return false;
		}
	}
}
