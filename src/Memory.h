#import <stdlib.h>
#import <string.h>

#import "Types.h"
#import "Compiler.h"

#define self Memory

#ifndef Memory_BoundaryChecks
#define Memory_BoundaryChecks 1
#endif

#ifndef Memory_PointerChecks
#define Memory_PointerChecks 0
#endif

#ifndef Memory_OutOfMemoryChecks
#define Memory_OutOfMemoryChecks 0
#endif

// @exc NullPointer
// @exc OutOfBounds
// @exc OutOfMemory
// @exc Overlapping

extern void *_etext;
extern void *__data_start;

static inline rsdef(bool, IsRoData, void *ptr) {
	return
		ptr > (void *) &_etext &&
		ptr < (void *) &__data_start;
}

rsdef(bool, Overlaps, void *dst, const void *src, size_t dstlen, size_t srclen);
__malloc rsdef(void *, Alloc, size_t size);
sdef(void, FreePtr, void *pMem);
__malloc rsdef(void *, Realloc, void *pMem, size_t size);
sdef(void, Copy, void *restrict pDest, const void *restrict pSource, size_t len);
__malloc rsdef(void *, Clone, void *pSource, size_t size);
rsdef(bool, Equals, void *ptr1, void *ptr2, size_t len);
rsdef(void *, Move, void *pDest, void *pSource, size_t len);

#define Memory_Free(pMem)                \
	do {                                 \
		Memory_FreePtr((void *) (pMem)); \
		(pMem) = NULL;                   \
	} while(0)

#define New(X) \
	(X *) Memory_Alloc(sizeof(X))

#undef self
