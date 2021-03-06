#import "Memory.h"
#import "Exception.h"

#define self Memory

/*
 * Taken from Valgrind.
 * Copyright (C) 2000-2010 Julian Seward
 *
 * Figure out if [dst .. dst+dstlen-1] overlaps with
 *               [src .. src+srclen-1].
 *
 * We assume that the address ranges do not wrap around (which is safe since on
 * Linux addresses >= 0xC0000000 are not accessible and the program will segfault
 * in this circumstance, presumably).
 */

rsdef(bool, Overlaps, void *dst, const void *src, size_t dstlen, size_t srclen) {
	if (dstlen == 0 || srclen == 0) {
		return false;
	}

	void *loS = (void *) src;
	void *loD = (void *) dst;
	void *hiS = loS + srclen - 1;
	void *hiD = loD + dstlen - 1;

	/* Figure out if [loS .. hiS] overlaps with [loD .. hiD]. */
	if (loS < loD) {
		return !(hiS < loD);
	} else if (loD < loS) {
		return !(hiD < loS);
	}

	/* They start at same place. Since we know neither of them
	 * has zero length, they must overlap.
	 */

	return true;
}

static Memory mem = { .impl = NULL };

void Memory0(Memory _mem) {
	mem = _mem;
}

__malloc rsdef(void *, New, size_t size) {
	assert(mem.impl != NULL);

	ref(Chunk) *chunk = delegate(mem, allocate, size);

	if (chunk == NULL) {
		throw(OutOfMemory);
	}

	return chunk->data;
}

sdef(void, Destroy, void *data) {
	assert(data     != NULL);
	assert(mem.impl != NULL);

	ref(Chunk) *chunk = data - sizeof(ref(Chunk));

	delegate(mem, release, chunk);
}

__malloc rsdef(void *, Resize, void *data, size_t size) {
	assert(data     != NULL);
	assert(mem.impl != NULL);

	ref(Chunk) *chunk = data - sizeof(ref(Chunk));

	chunk = delegate(mem, resize, chunk, size);

	if (chunk == NULL) {
		throw(OutOfMemory);
	}

	return chunk->data;
}

rsdef(size_t, GetSize, void *data) {
	assert(data != NULL);
	ref(Chunk) *chunk = data - sizeof(ref(Chunk));
	return chunk->size;
}

sdef(void, Copy, void *restrict pDest, const void *restrict pSource, size_t len) {
	if (len == 0) {
		return;
	}

	if (Memory_Overlaps(pDest, pSource, len, len)) {
		throw(Overlapping);
	}

	assert(pDest != NULL && pSource != NULL);
	memcpy(pDest, pSource, len);
}

rsdef(bool, Equals, void *ptr1, void *ptr2, size_t len) {
	if (len == 0) {
		return true;
	}

	assert(ptr1 != NULL && ptr2 != NULL);

	return memcmp(ptr1, ptr2, len) == 0;
}

sdef(void, Move, void *pDest, void *pSource, size_t len) {
	if (len == 0) {
		return;
	}

	assert(pDest != NULL && pSource != NULL);
	memmove(pDest, pSource, len);
}
