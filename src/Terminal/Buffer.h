#import "../Block.h"
#import "../Unicode.h"
#import "../Terminal.h"

#undef self
#define self Terminal_Buffer

record(ref(Chunk)) {
	String value;
	int color;
	int font;
	size_t line;
};

class(self) {
	Array(ref(Chunk), *chunks);
	Terminal *term;
	size_t spacing;
	size_t lines;
	size_t max;
};

def(void, Init, Terminal *term, size_t spacing);
def(void, Destroy);
def(size_t, Count);
def(size_t, AddChunk, ref(Chunk) chunk);
def(void, ChangeAttr, size_t id, int color, int font);
def(void, ChangeValue, size_t id, String s);
def(void, Redraw);
def(void, Clear);
