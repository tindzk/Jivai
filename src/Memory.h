#import "Object.h"

#define self Memory

#define exc(...)
exc(OutOfMemory)
exc(Overlapping)

#ifndef Memory_PageSize
#define Memory_PageSize 4096
#endif

record(ref(Chunk)) {
	size_t size; /* Usable size. */
	char data[];
};

Interface(self) {
	ref(Chunk)* (*allocate)(Instance $this, size_t size);
	void        (*release) (Instance $this, ref(Chunk) *chunk);
	ref(Chunk)* (*resize)  (Instance $this, ref(Chunk) *chunk, size_t size);
};

extern void *_etext;
extern void *__data_start;

static inline rsdef(bool, IsRoData, void *ptr) {
	return ptr > (void *) &_etext
		&& ptr < (void *) &__data_start;
}

rsdef(bool, Overlaps, void *dst, const void *src, size_t dstlen, size_t srclen);
void Memory0(Memory _mem);
__malloc rsdef(void *, New, size_t size);
sdef(void, Destroy, void *data);
__malloc rsdef(void *, Resize, void *data, size_t size);
rsdef(size_t, GetSize, void *data);
sdef(void, Copy, void *restrict pDest, const void *restrict pSource, size_t len);
rsdef(bool, Equals, void *ptr1, void *ptr2, size_t len);
sdef(void, Move, void *pDest, void *pSource, size_t len);

#undef self
