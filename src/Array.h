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

#define Array_Init(this, cnt)   \
	do {                        \
		if ((cnt) > 0) {        \
			(this)->buf =       \
				Memory_Alloc((cnt) * sizeof(typeof((this)->buf[0]))); \
		} else {                \
			(this)->buf = NULL; \
		}                       \
		(this)->len  = 0;       \
		(this)->size = cnt;     \
	} while(0)

#define Array_Destroy(this, freeItem)              \
	do {                                           \
		for (size_t i = 0; i < (this)->len; i++) { \
			freeItem(&(this)->buf[i]);              \
		}                                          \
		(this)->len  = 0;                          \
		(this)->size = 0;                          \
		if ((this)->buf != NULL) {                 \
			Memory_Free((this)->buf);              \
		}                                          \
	} while(0)

#define Array_Resize(this, cnt)    \
	do {                           \
		if ((this)->buf == NULL) { \
			(this)->buf =          \
				Memory_Alloc((cnt) * sizeof(typeof((this)->buf[0]))); \
		} else {                   \
			(this)->buf =          \
				Memory_Realloc((this)->buf, (cnt) * sizeof(typeof((this)->buf[0]))); \
		}                          \
		(this)->size = cnt;        \
		if ((this)->len > (cnt)) { \
			(this)->len = cnt;     \
		}                          \
	} while(0)

#define Array_Align(this, cnt)                              \
	do {                                                    \
		if ((cnt) > 0) {                                    \
			if ((this)->len == 0 || (cnt) > (this)->size) { \
				Array_Resize(this, cnt);                    \
			}                                               \
		}                                                   \
	} while(0)

#define Array_Push(this, data)              \
	do {                                    \
		Array_Align(this, (this)->len + 1); \
		(this)->buf[(this)->len] = data;    \
		(this)->len++;                      \
	} while(0)

#endif
