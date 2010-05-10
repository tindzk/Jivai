#include "Selection.h"

void Terminal_Selection_Init(Terminal_Selection *this, Terminal *term) {
	this->term = term;
	this->cur = 0;

	Terminal_Buffer_Init(&this->termbuf, term, 2);
	Terminal_HideCursor(this->term);
}

void Terminal_Selection_Destroy(Terminal_Selection *this) {
	Terminal_ShowCursor(this->term);
	Terminal_Buffer_Destroy(&this->termbuf);
}

void Terminal_Selection_OnSelect(Terminal_Selection *this, size_t id) {
	Terminal_Buffer_ChangeAttr(&this->termbuf,
		this->cur,
		Terminal_Color_Normal,
		Terminal_Font_Normal);

	Terminal_Buffer_ChangeAttr(&this->termbuf,
		id,
		Terminal_Color_BackgroundCyan | Terminal_Color_ForegroundWhite,
		Terminal_Font_Normal);

	this->cur = id;
}

void Terminal_Selection_OnDraw(Terminal_Selection *this, size_t id, String caption, bool selected) {
	String index = StackString(5);

	String_Append(&index, String("("));
	String_Append(&index, Integer_ToString(id));
	String_Append(&index, String(") "));

	int color = selected
		? Terminal_Color_BackgroundCyan | Terminal_Color_ForegroundWhite
		: Terminal_Color_Normal;

	Terminal_Buffer_SetAttr(&this->termbuf,
		color,
		Terminal_Font_Normal);

	Terminal_Buffer_Print(&this->termbuf, index);
	Terminal_Buffer_Print(&this->termbuf, caption);
}

size_t Terminal_Selection_Add(Terminal_Selection *this, String caption, bool selected) {
	size_t id;

	id = Terminal_Buffer_NewChunk(&this->termbuf);
	Terminal_Selection_OnDraw(this, id, caption, selected);

	return id;
}

ssize_t Terminal_Selection_GetSel(Terminal_Selection *this) {
	while (true) {
		Terminal_Key key = Terminal_ReadKey(this->term);

		if (key.t == Terminal_KeyType_Left) {
			ssize_t new = this->cur - 1;

			if (new >= 0 && (size_t) new < this->termbuf.chunks.len) {
				Terminal_Selection_OnSelect(this, new);
			}
		} else if (key.t == Terminal_KeyType_Right) {
			size_t new = this->cur + 1;

			if (new < this->termbuf.chunks.len) {
				Terminal_Selection_OnSelect(this, new);
			}
		} else if (key.t == Terminal_KeyType_Home) {
			if (this->termbuf.chunks.len > 0) {
				Terminal_Selection_OnSelect(this, 0);
			}
		} else if (key.t == Terminal_KeyType_End) {
			if (this->termbuf.chunks.len > 0) {
				Terminal_Selection_OnSelect(this, this->termbuf.chunks.len - 1);
			}
		} else if (Char_IsDigit(key.c)) {
			size_t new = Char_ParseDigit(key.c);

			if (new < this->termbuf.chunks.len) {
				Terminal_Selection_OnSelect(this, new);
			}
		} else if (key.c == CTRLKEY('c')) {
			return -1;
		} else if (key.c == '\n') {
			Terminal_Buffer_Clear(&this->termbuf);
			return this->cur;
		}
	}

	return 0;
}
