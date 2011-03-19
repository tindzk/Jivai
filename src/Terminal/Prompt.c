#import "Prompt.h"

#define self Terminal_Prompt

def(void, Init, Terminal *term) {
	this->term    = term;
	this->termbuf = Terminal_Buffer_New(term, 1);
}

def(void, Destroy) {
	Terminal_Buffer_Destroy(&this->termbuf);
}

def(bool, Ask, OmniString msg) {
	Terminal_Buffer_Chunk chunk;

	chunk.color = Terminal_Color_Normal;
	chunk.font  = Terminal_Font_Normal;
	chunk.value = String_ToCarrier(msg);

	Terminal_Buffer_AddChunk(&this->termbuf, chunk);

	chunk.value = String_ToCarrier($$("[y/n] "));

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
