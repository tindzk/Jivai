#import "Memory.h"
#import "Exception.h"

static ExceptionManager *exc;

void Memory0(void *e) {
	exc = e;
}

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

bool Memory_Overlaps(void *dst, const void *src, size_t dstlen, size_t srclen) {
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

void* __malloc Memory_Alloc(size_t size) {
#if Memory_BoundaryChecks
	if (size == 0 || size > MaxValue(size_t)) {
		throw(exc, excOutOfBounds);
	}
#endif

	void *pMem = malloc(size);

#if Memory_OutOfMemoryChecks
	if (pMem == NULL) {
		throw(exc, excOutOfMemory);
	}
#endif

	return pMem;
}

void Memory_FreePtr(void *pMem) {
#if Memory_PointerChecks
	if (pMem == NULL) {
		throw(exc, excNullPointer);
	}
#endif

	free(pMem);
}

void* __malloc Memory_Realloc(void *pMem, size_t size) {
#if Memory_BoundaryChecks
	if (size == 0 || size > MaxValue(size_t)) {
		throw(exc, excOutOfBounds);
	}
#endif

#if Memory_PointerChecks
	if (pMem == NULL) {
		throw(exc, excOutOfMemory);
	}
#endif

	void *res = realloc(pMem, size);

#if Memory_OutOfMemoryChecks
	if (res == NULL) {
		throw(exc, excOutOfMemory);
	}
#endif

	return res;
}

void Memory_Copy(void *restrict pDest, const void *restrict pSource, size_t len) {
#if Memory_BoundaryChecks
	if (len == 0 || len > MaxValue(size_t)) {
		throw(exc, excOutOfBounds);
	}
#endif

	if (Memory_Overlaps(pDest, pSource, len, len)) {
		throw(exc, excOverlapping);
	}

#if Memory_PointerChecks
	if (pDest == NULL || pSource == NULL) {
		throw(exc, excOutOfMemory);
	}
#endif

	memcpy(pDest, pSource, len);
}

void* __malloc Memory_Clone(void *pSource, size_t size) {
#if Memory_BoundaryChecks
	if (size == 0 || size > MaxValue(size_t)) {
		throw(exc, excOutOfBounds);
	}
#endif

#if Memory_PointerChecks
	if (pSource == NULL) {
		throw(exc, excOutOfMemory);
	}
#endif

	void *pDest = malloc(size);

#if Memory_OutOfMemoryChecks
	if (pDest == NULL) {
		throw(exc, excOutOfMemory);
	}
#endif

	memcpy(pDest, pSource, size);

	return pDest;
}

bool Memory_Equals(void *ptr1, void *ptr2, size_t len) {
	if (len == 0) {
		return true;
	}

#if Memory_BoundaryChecks
	if (len > MaxValue(size_t)) {
		throw(exc, excOutOfBounds);
	}
#endif

#if Memory_PointerChecks
	if (ptr1 == NULL || ptr2 == NULL) {
		throw(exc, excOutOfMemory);
	}
#endif

	return memcmp(ptr1, ptr2, len) == 0;
}

void* Memory_Move(void *pDest, void *pSource, size_t len) {
#if Memory_BoundaryChecks
	if (len == 0 || len > MaxValue(size_t)) {
		throw(exc, excOutOfBounds);
	}
#endif

#if Memory_PointerChecks
	if (pDest == NULL || pSource == NULL) {
		throw(exc, excOutOfMemory);
	}
#endif

	return memmove(pDest, pSource, len);
}
