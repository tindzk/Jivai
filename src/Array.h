#ifndef ARRAY_H
#define ARRAY_H

#include "Memory.h"

#define Array(type, name) \
	struct {              \
		size_t len;       \
		size_t size;      \
		type buf[0];      \
	} name

#define Array_ItemSize(this) \
	sizeof(typeof((this)->buf[0]))

#define Array_Size(arr, len) \
	(sizeof(*arr) + (len) * Array_ItemSize(arr))

#define Array_Init(this, items)                               \
	do {                                                      \
		(this) = Memory_Alloc(Array_Size(this, (items) + 1)); \
		(this)->len  = 0;                                     \
		(this)->size = items;                                 \
	} while(0)

#define Array_Foreach(this, cb)                    \
	do {                                           \
		for (size_t i = 0; i < (this)->len; i++) { \
			cb(&(this)->buf[i]);                   \
		}                                          \
	} while(0)

#define Array_Destroy(this) \
	Memory_Free(this)

#define Array_Resize(this, items)                               \
	do {                                                        \
		(this) = Memory_Realloc(this, Array_Size(this, items)); \
		(this)->size = items;                                   \
		if ((this)->len > items) {                              \
			(this)->len = items;                                \
		}                                                       \
	} while(0)

#define Array_Align(this, items)                               \
	do {                                                       \
		if ((items) > 0) {                                     \
			if ((this)->size == 0 || (items) > (this)->size) { \
				Array_Resize(this, items);                     \
			}                                                  \
		}                                                      \
	} while(0)

#define Array_Push(this, data)              \
	do {                                    \
		Array_Align(this, (this)->len + 1); \
		(this)->buf[(this)->len] = data;    \
		(this)->len++;                      \
	} while(0)

#define Array_Sort(this, cmp) \
	qsort((this)->buf, (this)->len, Array_ItemSize(this), (void *) cmp)

#endif
