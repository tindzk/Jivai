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
 * We assume that the address ranges do not wrap around (which is
 * safe since on Linux addresses >= 0xC0000000 are not accessible
 * and the program will segfault in this circumstance, presumably).
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

__malloc rsdef(void *, Alloc, size_t size) {
#if Memory_BoundaryChecks
	if (size == 0) {
		throw(OutOfBounds);
	}
#endif

	void *pMem = malloc(size);

#if Memory_OutOfMemoryChecks
	if (pMem == NULL) {
		throw(OutOfMemory);
	}
#endif

	return pMem;
}

sdef(void, FreePtr, void *pMem) {
#if Memory_PointerChecks
	if (pMem == NULL) {
		throw(NullPointer);
	}
#endif

	free(pMem);
}

__malloc rsdef(void *, Realloc, void *pMem, size_t size) {
#if Memory_BoundaryChecks
	if (size == 0) {
		throw(OutOfBounds);
	}
#endif

#if Memory_PointerChecks
	if (pMem == NULL) {
		throw(OutOfMemory);
	}
#endif

	void *res = realloc(pMem, size);

#if Memory_OutOfMemoryChecks
	if (res == NULL) {
		throw(OutOfMemory);
	}
#endif

	return res;
}

sdef(void, Copy, void *restrict pDest, const void *restrict pSource, size_t len) {
#if Memory_BoundaryChecks
	if (len == 0) {
		throw(OutOfBounds);
	}
#endif

	if (Memory_Overlaps(pDest, pSource, len, len)) {
		throw(Overlapping);
	}

#if Memory_PointerChecks
	if (pDest == NULL || pSource == NULL) {
		throw(OutOfMemory);
	}
#endif

	memcpy(pDest, pSource, len);
}

__malloc rsdef(void *, Clone, void *pSource, size_t size) {
#if Memory_BoundaryChecks
	if (size == 0) {
		throw(OutOfBounds);
	}
#endif

#if Memory_PointerChecks
	if (pSource == NULL) {
		throw(OutOfMemory);
	}
#endif

	void *pDest = malloc(size);

#if Memory_OutOfMemoryChecks
	if (pDest == NULL) {
		throw(OutOfMemory);
	}
#endif

	memcpy(pDest, pSource, size);

	return pDest;
}

rsdef(bool, Equals, void *ptr1, void *ptr2, size_t len) {
	if (len == 0) {
		return true;
	}

#if Memory_PointerChecks
	if (ptr1 == NULL || ptr2 == NULL) {
		throw(OutOfMemory);
	}
#endif

	return memcmp(ptr1, ptr2, len) == 0;
}

sdef(void, Move, void *pDest, void *pSource, size_t len) {
#if Memory_BoundaryChecks
	if (len == 0) {
		throw(OutOfBounds);
	}
#endif

#if Memory_PointerChecks
	if (pDest == NULL || pSource == NULL) {
		throw(OutOfMemory);
	}
#endif

	memmove(pDest, pSource, len);
}
