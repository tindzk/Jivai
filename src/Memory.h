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

static inline bool Memory_IsRoData(void *ptr) {
	return
		ptr > (void *) &_etext &&
		ptr < (void *) &__data_start;
}

void Memory0(void *e);
bool Memory_Overlaps(void *dst, const void *src, size_t dstlen, size_t srclen);
void* __malloc Memory_Alloc(size_t size);
void Memory_FreePtr(void *pMem);
void* __malloc Memory_Realloc(void *pMem, size_t size);
void Memory_Copy(void *restrict pDest, const void *restrict pSource, size_t len);
void* __malloc Memory_Clone(void *pSource, size_t len);
bool Memory_Equals(void *ptr1, void *ptr2, size_t num);
void* Memory_Move(void *pDest, void *pSource, size_t num);

#define Memory_Free(pMem)                \
	do {                                 \
		Memory_FreePtr((void *) (pMem)); \
		(pMem) = NULL;                   \
	} while(0)

#define New(X) \
	(X *) Memory_Alloc(sizeof(X))

#undef self
