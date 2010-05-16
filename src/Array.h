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

#define Array_ResetRange(this, offset, cnt, freeItem)    \
	do {                                                 \
		for (size_t i = offset; i < offset + cnt; i++) { \
			freeItem(&(this)->buf[i]);                   \
		}                                                \
		(this)->len -= cnt;                              \
	} while(0)

#define Array_Reset(this, cnt, freeItem) \
	Array_ResetRange(this, (this)->len - cnt, cnt, freeItem)

#define Array_Destroy(this, freeItem)                     \
	do {                                                  \
		Array_ResetRange(this, 0, (this)->len, freeItem); \
		Array_CustomDestroy((void *) this);               \
	} while(0)

#define Array_Resize(this, cnt, freeItem)      \
	do {                                       \
		if ((this)->len > cnt) {               \
			Array_ResetRange(this, cnt,        \
				(this)->len - cnt, freeItem);  \
		}                                      \
		Array_CustomResize((void *) this, cnt, \
			sizeof(typeof((this)->buf[0])));   \
	} while(0)

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
