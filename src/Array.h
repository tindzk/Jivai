#ifndef ARRAY_H
#define ARRAY_H

#include "NULL.h"
#include "Memory.h"

#define Array(type, name) \
	struct {              \
		size_t len;       \
		size_t size;      \
		type *buf;        \
	} name

typedef Array(void *, Array);

void Array_CustomInit(Array *this, size_t len, size_t size);
void Array_CustomDestroy(Array *this);
void Array_CustomResize(Array *this, size_t len, size_t size);
void Array_CustomAlign(Array *this, size_t len, size_t size);

#define Array_Init(this, cnt) \
	Array_CustomInit((void *) this, cnt, sizeof(typeof((this)->buf[0])))

#define Array_Destroy(this, freeItem)          \
	for (size_t i = 0; i < (this)->len; i++) { \
		freeItem(&(this)->buf[i]);             \
	}                                          \
	Array_CustomDestroy((void *) this);

#define Array_Resize(this, cnt) \
	Array_CustomResize((void *) this, cnt, sizeof(typeof((this)->buf[0])))

#define Array_Align(this, cnt) \
	Array_CustomAlign((void *) this, cnt, sizeof(typeof((this)->buf[0])))

#define Array_Push(this, data)              \
	do {                                    \
		Array_Align(this, (this)->len + 1); \
		(this)->buf[(this)->len] = data;    \
		(this)->len++;                      \
	} while(0)

#define Array_Sort(this, cmp) \
	qsort((this)->buf, (this)->len, sizeof(*(this)->buf), (void *) cmp)

#endif
