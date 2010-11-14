#import <stdlib.h>
#import <stdint.h>
#import <string.h>

#import "Types.h"
#import "Compiler.h"
#import "Exceptions.h"

#undef self
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

enum {
	excNullPointer = excOffset,
	excOutOfBounds,
	excOutOfMemory
};

void Memory0(void *e);
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

#define Memory_CopyObject(pDest, pSource) \
	Memory_Copy(pDest, pSource, sizeof(*(pSource)))

#define Memory_CloneObject(obj) \
	Memory_Clone(&(obj), sizeof(obj))
