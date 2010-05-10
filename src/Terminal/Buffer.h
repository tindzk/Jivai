#ifndef TERMINAL_BUFFER_H
#define TERMINAL_BUFFER_H

#include "../Terminal.h"
#include "../Unicode.h"

typedef struct {
	String value;
	int color;
	int font;
} Terminal_Buffer_Chunk;

typedef struct {
	Array(Terminal_Buffer_Chunk, chunks);
	Terminal *term;
	size_t spacing;
} Terminal_Buffer;

void Terminal_Buffer_Init(Terminal_Buffer *this, Terminal *term, size_t spacing);
void Terminal_Buffer_Destroy(Terminal_Buffer *this);
size_t Terminal_Buffer_NewChunk(Terminal_Buffer *this);
void Terminal_Buffer_SelectChunk(Terminal_Buffer *this, size_t id);
void Terminal_Buffer_ChangeAttr(Terminal_Buffer *this, size_t id, int color, int font);
void Terminal_Buffer_ChangeValue(Terminal_Buffer *this, size_t id, String s);
void Terminal_Buffer_SetAttr(Terminal_Buffer *this, int color, int font);
void Terminal_Buffer_Print(Terminal_Buffer *this, String s);
void Terminal_Buffer_Clear(Terminal_Buffer *this);

#endif
