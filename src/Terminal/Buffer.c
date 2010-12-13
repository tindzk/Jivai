#import "Buffer.h"

#define self Terminal_Buffer

def(void, Init, Terminal *term, size_t spacing) {
	this->term    = term;
	this->spacing = spacing;
	this->lines   = 0;
	this->max     = Terminal_GetSize().cols;

	Array_Init(this->chunks, 0);
}

def(void, Destroy) {
	call(Clear);

	foreach (chunk, this->chunks) {
		String_Destroy(&chunk->value);
	}

	Array_Destroy(this->chunks);
}

static def(size_t, GetCurrentLineLength) {
	size_t len = 0;

	foreach (chunk, this->chunks) {
		if (chunk->line == this->lines) {
			len += Unicode_Count(chunk->value);

			if (!isLast(chunk, this->chunks)) {
				len += this->spacing;
			}
		}
	}

	return len;
}

static def(size_t, CountChunksOnCurrentLine) {
	size_t chunks = 0;

	foreach (chunk, this->chunks) {
		if (chunk->line == this->lines) {
			chunks++;
		}
	}

	return chunks;
}

def(size_t, Count) {
	return this->chunks->len;
}

def(size_t, AddChunk, ref(Chunk) chunk) {
	size_t len = call(GetCurrentLineLength);

	size_t width = Unicode_Count(chunk.value);

	if (len + width > this->max) {
		this->lines++;
		Terminal_Print(this->term, '\n');
	} else {
		if (call(CountChunksOnCurrentLine) > 0) {
			repeat (this->spacing) {
				Terminal_Print(this->term, ' ');
			}
		}
	}

	chunk.line = this->lines;

	if (width > this->max) {
		if (this->max > 3) {
			Unicode_Shrink(&chunk.value, this->max - 3);
			String_Append(&chunk.value, $("..."));
		}
	}

	Terminal_Print(this->term,
		chunk.color,
		chunk.font,
		chunk.value);

	Array_Push(this->chunks, chunk);

	return this->chunks->len - 1;
}

def(void, ChangeAttr, size_t id, int color, int font) {
	if (id >= this->chunks->len) {
		return;
	}

	this->chunks->buf[id].color = color;
	this->chunks->buf[id].font  = font;
}

def(void, ChangeValue, size_t id, String s) {
	if (id >= this->chunks->len) {
		return;
	}

	String_Copy(&this->chunks->buf[id].value, s);
}

def(void, Redraw) {
	Terminal_Print(this->term,      '\n');
	Terminal_MoveUp(this->term,     this->lines + 1);
	Terminal_DeleteLine(this->term, this->lines + 1);

	this->lines = 0;

	size_t len = 0; /* Line length. */
	bool first = true;

	foreach (chunk, this->chunks) {
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
				repeat (this->spacing) {
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

def(void, Clear) {
	if (this->chunks->len == 0) {
		return;
	}

	Terminal_Print(this->term,      '\n');
	Terminal_MoveUp(this->term,     this->lines + 1);
	Terminal_DeleteLine(this->term, this->lines + 1);

	this->lines = 0;

	reverse (i, this->chunks->len) {
		String_Destroy(&this->chunks->buf[i].value);
	}

	this->chunks->len = 0;
}
