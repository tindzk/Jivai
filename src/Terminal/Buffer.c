#import "Buffer.h"

void Terminal_Buffer_Init(Terminal_Buffer *this, Terminal *term, size_t spacing) {
	this->term    = term;
	this->spacing = spacing;
	this->lines   = 0;
	this->max     = Terminal_GetSize().cols;

	Array_Init(this->chunks, 0);
}

void Terminal_Buffer_Destroy(Terminal_Buffer *this) {
	Terminal_Buffer_Clear(this);

	Array_Foreach(this->chunks, ^(Terminal_Buffer_Chunk *item) {
		String_Destroy(&item->value);
	});

	Array_Destroy(this->chunks);
}

static size_t Terminal_Buffer_GetCurrentLineLength(Terminal_Buffer *this) {
	size_t len = 0;

	for (size_t i = 0; i < this->chunks->len; i++) {
		Terminal_Buffer_Chunk *chunk = &this->chunks->buf[i];

		if (chunk->line == this->lines) {
			len += Unicode_Count(chunk->value);

			if (i + 1 != this->chunks->len) {
				len += this->spacing;
			}
		}
	}

	return len;
}

static size_t Terminal_Buffer_CountChunksOnCurrentLine(Terminal_Buffer *this) {
	size_t chunks = 0;

	for (size_t i = 0; i < this->chunks->len; i++) {
		Terminal_Buffer_Chunk *chunk = &this->chunks->buf[i];

		if (chunk->line == this->lines) {
			chunks++;
		}
	}

	return chunks;
}

size_t Terminal_Buffer_Count(Terminal_Buffer *this) {
	return this->chunks->len;
}

size_t Terminal_Buffer_AddChunk(Terminal_Buffer *this, Terminal_Buffer_Chunk chunk) {
	size_t len = Terminal_Buffer_GetCurrentLineLength(this);

	size_t width = Unicode_Count(chunk.value);

	if (len + width > this->max) {
		this->lines++;
		Terminal_Print(this->term, '\n');
	} else {
		if (Terminal_Buffer_CountChunksOnCurrentLine(this) > 0) {
			for (size_t i = this->spacing; i > 0; i--) {
				Terminal_Print(this->term, ' ');
			}
		}
	}

	chunk.line = this->lines;

	if (width > this->max) {
		if (this->max > 3) {
			Unicode_Shrink(&chunk.value, this->max - 3);
			String_Append(&chunk.value, String("..."));
		}
	}

	Terminal_Print(this->term,
		chunk.color,
		chunk.font,
		chunk.value);

	Array_Push(this->chunks, chunk);

	return this->chunks->len - 1;
}

void Terminal_Buffer_ChangeAttr(Terminal_Buffer *this, size_t id, int color, int font) {
	if (id >= this->chunks->len) {
		return;
	}

	this->chunks->buf[id].color = color;
	this->chunks->buf[id].font  = font;
}

void Terminal_Buffer_ChangeValue(Terminal_Buffer *this, size_t id, String s) {
	if (id >= this->chunks->len) {
		return;
	}

	String_Copy(&this->chunks->buf[id].value, s);
}

void Terminal_Buffer_Redraw(Terminal_Buffer *this) {
	Terminal_Print(this->term,      '\n');
	Terminal_MoveUp(this->term,     this->lines + 1);
	Terminal_DeleteLine(this->term, this->lines + 1);

	this->lines = 0;

	size_t len = 0; /* Line length. */
	bool first = true;

	for (size_t i = 0; i < this->chunks->len; i++) {
		Terminal_Buffer_Chunk *chunk = &this->chunks->buf[i];

		size_t width = Unicode_Count(chunk->value);

		/* Item width including spacing. */
		size_t fullItemWidth = width;
		if (!first) {
			fullItemWidth += this->spacing;
		}

		if (len + fullItemWidth > this->max) {
			this->lines++;

			first = true;
			len   = 0;

			Terminal_Print(this->term, '\n');
		} else {
			if (!first) {
				for (size_t i = this->spacing; i > 0; i--) {
					Terminal_Print(this->term, ' ');
				}
			}

			len  += fullItemWidth;
			first = false;
		}

		chunk->line = this->lines;

		Terminal_Print(this->term,
			chunk->color,
			chunk->font,
			chunk->value);
	}
}

void Terminal_Buffer_Clear(Terminal_Buffer *this) {
	if (this->chunks->len == 0) {
		return;
	}

	Terminal_Print(this->term,      '\n');
	Terminal_MoveUp(this->term,     this->lines + 1);
	Terminal_DeleteLine(this->term, this->lines + 1);

	this->lines = 0;

	for (ssize_t i = this->chunks->len - 1; i >= 0; i--) {
		String_Destroy(&this->chunks->buf[i].value);
	}

	this->chunks->len = 0;
}
