#import "../Block.h"
#import "../Unicode.h"
#import "../Terminal.h"

typedef struct {
	String value;
	int color;
	int font;
	size_t line;
} Terminal_Buffer_Chunk;

typedef struct {
	Array(Terminal_Buffer_Chunk, *chunks);
	Terminal *term;
	size_t spacing;
	size_t lines;
	size_t max;
} Terminal_Buffer;

void Terminal_Buffer_Init(Terminal_Buffer *this, Terminal *term, size_t spacing);
void Terminal_Buffer_Destroy(Terminal_Buffer *this);
size_t Terminal_Buffer_Count(Terminal_Buffer *this);
size_t Terminal_Buffer_AddChunk(Terminal_Buffer *this, Terminal_Buffer_Chunk chunk);
void Terminal_Buffer_ChangeAttr(Terminal_Buffer *this, size_t id, int color, int font);
void Terminal_Buffer_ChangeValue(Terminal_Buffer *this, size_t id, String s);
void Terminal_Buffer_Redraw(Terminal_Buffer *this);
void Terminal_Buffer_Clear(Terminal_Buffer *this);
