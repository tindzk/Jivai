#import "Buffer.h"

void Terminal_Buffer_Init(Terminal_Buffer *this, Terminal *term, size_t spacing) {
	this->term = term;
	this->spacing = spacing;

	Array_Init(this->chunks, 0);
}

void Terminal_Buffer_Destroy(Terminal_Buffer *this) {
	Terminal_Buffer_Clear(this);

	Array_Foreach(this->chunks, ^(Terminal_Buffer_Chunk *item) {
		String_Destroy(&item->value);
	});

	Array_Destroy(this->chunks);
}

size_t Terminal_Buffer_NewChunk(Terminal_Buffer *this) {
	if (this->chunks->len > 0) {
		for (size_t i = this->spacing; i > 0; i--) {
			Terminal_Print(this->term,
				Terminal_Color_Normal,
				Terminal_Font_Normal,
				String(" "));
		}
	}

	Terminal_Buffer_Chunk chunk;

	chunk.value = HeapString(0);
	chunk.color = Terminal_Color_Normal;
	chunk.font = Terminal_Font_Normal;

	Array_Push(this->chunks, chunk);
	return this->chunks->len - 1;
}

void Terminal_Buffer_ChangeAttr(Terminal_Buffer *this, size_t id, int color, int font) {
	if (id >= this->chunks->len) {
		return;
	}

	this->chunks->buf[id].color = color;
	this->chunks->buf[id].font  = font;

	size_t total = 0;

	for (ssize_t i = this->chunks->len - 1; i >= (ssize_t) id; i--) {
		total += Unicode_Count(this->chunks->buf[i].value);

		if ((size_t) i != this->chunks->len - 1) {
			total += this->spacing;
		}
	}

	Terminal_MoveLeft(this->term, total);

	for (size_t i = id; i < this->chunks->len; i++) {
		Terminal_Print(this->term,
			this->chunks->buf[i].color,
			this->chunks->buf[i].font,
			this->chunks->buf[i].value);

		if (i != this->chunks->len - 1) {
			for (size_t j = this->spacing; j > 0; j--) {
				Terminal_Print(this->term,
					Terminal_Color_Normal,
					Terminal_Font_Normal,
					String(" "));
			}
		}
	}
}

void Terminal_Buffer_ChangeValue(Terminal_Buffer *this, size_t id, String s) {
	if (id >= this->chunks->len) {
		return;
	}

	size_t total = 0;

	for (ssize_t i = this->chunks->len - 1; i >= (ssize_t) id; i--) {
		total += Unicode_Count(this->chunks->buf[i].value);

		if ((size_t) i != this->chunks->len - 1) {
			total += this->spacing;
		}
	}

	Terminal_MoveLeft(this->term, total);
	Terminal_DeleteUntilEol(this->term);

	String_Copy(&this->chunks->buf[id].value, s);

	for (size_t i = id; i < this->chunks->len; i++) {
		Terminal_Print(this->term,
			this->chunks->buf[i].color,
			this->chunks->buf[i].font,
			this->chunks->buf[i].value);

		if (i != this->chunks->len - 1) {
			for (size_t j = this->spacing; j > 0; j--) {
				Terminal_Print(this->term,
					Terminal_Color_Normal,
					Terminal_Font_Normal,
					String(" "));
			}
		}
	}
}

void Terminal_Buffer_SetAttr(Terminal_Buffer *this, int color, int font) {
	if (this->chunks->len == 0) {
		return;
	}

	this->chunks->buf[this->chunks->len - 1].color = color;
	this->chunks->buf[this->chunks->len - 1].font  = font;
}

void Terminal_Buffer_Print(Terminal_Buffer *this, String s) {
	if (this->chunks->len == 0) {
		return;
	}

	String_Append(
		&this->chunks->buf[this->chunks->len - 1].value,
		s);

	Terminal_Print(this->term,
		this->chunks->buf[this->chunks->len - 1].color,
		this->chunks->buf[this->chunks->len - 1].font,
		s);
}

void Terminal_Buffer_Clear(Terminal_Buffer *this) {
	if (this->chunks->len == 0) {
		return;
	}

	size_t total = 0;

	for (ssize_t i = this->chunks->len - 1; i >= 0; i--) {
		total += Unicode_Count(this->chunks->buf[i].value);
		total += this->spacing;

		String_Destroy(&this->chunks->buf[i].value);
		this->chunks->len--;
	}

	Terminal_MoveLeft(this->term, total);
	Terminal_DeleteUntilEol(this->term);
}
