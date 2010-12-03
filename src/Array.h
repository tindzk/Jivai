#import "Memory.h"

#define Array_Sort(this, cmp) \
	qsort((this)->buf, (this)->len, sizeof((this)->buf[0]), (void *) cmp)

#define Array(type, name)                                                                   \
	record(name) {                                                                          \
		size_t len;                                                                         \
		size_t size;                                                                        \
		type buf[0];                                                                        \
	};                                                                                      \
	typedef union {                                                                         \
		name *object;                                                                       \
		GenericInstance generic;                                                            \
	} Instance(name) transparentUnion;                                                      \
	static inline name* tripleConcat(name, _, New)(size_t items) {                          \
		name *res = Memory_Alloc(sizeof(name) + items * sizeof(type));                      \
		res->len  = 0;                                                                      \
		res->size = items;                                                                  \
		return res;                                                                         \
	}                                                                                       \
	static inline void tripleConcat(name, _, Free)                                          \
		(Instance(name) $this)                                                              \
	{                                                                                       \
		Memory_Free($this.object);                                                          \
	}                                                                                       \
	static inline void tripleConcat(name, _, Resize)                                        \
		(union { name **object } transparentUnion $this, size_t items)                      \
	{                                                                                       \
		*$this.object = Memory_Realloc(*$this.object, sizeof(name) + items * sizeof(type)); \
		(*$this.object)->size = items;                                                      \
		if ((*$this.object)->len > items) {                                                 \
			(*$this.object)->len = items;                                                   \
		}                                                                                   \
	}                                                                                       \
	static inline void tripleConcat(name, _, Align)                                         \
		(union { name **object } transparentUnion $this, size_t items)                      \
	{                                                                                       \
		if (items > 0) {                                                                    \
			if ((*$this.object)->size == 0 || items > (*$this.object)->size) {              \
				tripleConcat(name, _, Resize)($this.object, items);                         \
			}                                                                               \
		}                                                                                   \
	}                                                                                       \
	static inline void tripleConcat(name, _, Push)                                          \
		(union { name **object } transparentUnion $this, type data)                         \
	{                                                                                       \
		tripleConcat(name, _, Align)($this.object, (*$this.object)->len + 1);               \
		(*$this.object)->buf[(*$this.object)->len] = data;                                  \
		(*$this.object)->len++;                                                             \
	}
