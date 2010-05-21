#ifndef MEMORY_H
#define MEMORY_H

#include "String.h"
#include "Exception.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

Exception_Export(Memory_NullPointerException);
Exception_Export(Memory_OutOfBoundsException);
Exception_Export(Memory_OutOfMemoryException);

void Memory0(ExceptionManager *e);

void* Memory_Alloc(size_t size);
void Memory_FreePtr(void *pMem);
void* Memory_Realloc(void *pMem, size_t size);
void Memory_Copy(void *pDest, void *pSource, size_t len);
void* Memory_Clone(void *pSource, size_t len);
bool Memory_Equals(void *ptr1, void *ptr2, size_t num);
void* Memory_Move(void *pDest, void *pSource, size_t num);

#define Memory_Free(pMem)                \
	do {                                 \
		Memory_FreePtr((void *) (pMem)); \
		(pMem) = NULL;                   \
	} while(0)

#define New(X) \
	(X *) Memory_Alloc(sizeof(X))

#define StackNew(X) \
	(X *) alloca(sizeof(X))

#define Memory_CopyObject(pDest, pSource) \
	Memory_Copy(pDest, pSource, sizeof(*(pSource)))

#define Memory_CloneObject(pSource) \
	Memory_Clone(pSource, sizeof(*(pSource)))

#endif
