#import "../Block.h"
#import "../Unicode.h"
#import "../Terminal.h"

#define self Terminal_Buffer

record(ref(Chunk)) {
	CarrierString value;
	int color;
	int font;
	size_t line;
};

Array(ref(Chunk), ref(Chunks));

class {
	ref(Chunks) *chunks;
	Terminal *term;
	size_t spacing;
	size_t lines;
	size_t max;
};

rsdef(self, New, Terminal *term, size_t spacing);
def(void, Destroy);
def(size_t, Count);
def(size_t, AddChunk, ref(Chunk) chunk);
def(void, ChangeAttr, size_t id, int color, int font);
def(void, ChangeValue, size_t id, CarrierString s);
def(void, Redraw);
def(void, Clear);

#undef self
