#import "Memory.h"

static ExceptionManager *exc;

void Memory0(ExceptionManager *e) {
	exc = e;
}

void* Memory_Alloc(size_t size) {
#if Memory_BoundaryChecks
	if (size == 0 || size > SIZE_MAX) {
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

void* Memory_Realloc(void *pMem, size_t size) {
#if Memory_BoundaryChecks
	if (size == 0 || size > SIZE_MAX) {
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
	if (len == 0 || len > SIZE_MAX) {
		throw(exc, excOutOfBounds);
	}
#endif

#if Memory_PointerChecks
	if (pDest == NULL || pSource == NULL) {
		throw(exc, excOutOfMemory);
	}
#endif

	memcpy(pDest, pSource, len);
}

void* Memory_Clone(void *pSource, size_t size) {
#if Memory_BoundaryChecks
	if (size == 0 || size > SIZE_MAX) {
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
	if (len > SIZE_MAX) {
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
	if (len == 0 || len > SIZE_MAX) {
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
