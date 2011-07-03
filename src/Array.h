#include <stdlib.h> /* qsort() */

#import "Memory.h"

#define Array_Sort(this, cmp) \
	qsort((this)->buf, (this)->len, sizeof((this)->buf[0]), (void *) cmp)

#define Array(type, name)                                                                \
	record(name) {                                                                       \
		size_t len;                                                                      \
		type buf[];                                                                      \
	};                                                                                   \
	Instance(name);                                                                      \
	static inline name* tripleConcat(name, _, New)(size_t items) {                       \
		name *res = Memory_New(sizeof(name) + items * sizeof(type));                     \
		res->len  = 0;                                                                   \
		return res;                                                                      \
	}                                                                                    \
	static inline size_t tripleConcat(name, _, GetSize)(InstName(name) $this) {          \
		return (Memory_GetSize($this.addr) - sizeof(name)) / sizeof(type);               \
	}                                                                                    \
	static inline void tripleConcat(name, _, Free)(InstName(name) $this) {               \
		Memory_Destroy($this.addr);                                                      \
	}                                                                                    \
	static inline void tripleConcat(name, _, Delete)(InstName(name) $this, size_t idx) { \
		if ($this.addr->len - idx - 1 > 0) {                                             \
			Memory_Move(                                                                 \
				 $this.addr->buf + idx,                                                  \
				 $this.addr->buf + idx + 1,                                              \
				($this.addr->len - idx - 1) * sizeof(type));                             \
		}                                                                                \
		$this.addr->len--;                                                               \
	}                                                                                    \
	static inline void tripleConcat(name, _, Resize)                                     \
		(union { name **addr; } transparentUnion $this, size_t items)                    \
	{                                                                                    \
		*$this.addr = Memory_Resize(*$this.addr,                                         \
			sizeof(name) + items * sizeof(type));                                        \
		if ((*$this.addr)->len > items) {                                                \
			(*$this.addr)->len = items;                                                  \
		}                                                                                \
	}                                                                                    \
	static inline void tripleConcat(name, _, Align)                                      \
		(union { name **addr; } transparentUnion $this, size_t items)                    \
	{                                                                                    \
		if (items > 0 && items > tripleConcat(name, _, GetSize)(*$this.addr)) {          \
			tripleConcat(name, _, Resize)($this.addr, items);                            \
		}                                                                                \
	}                                                                                    \
	static inline void tripleConcat(name, _, Push)                                       \
		(union { name **addr; } transparentUnion $this, type data)                       \
	{                                                                                    \
		tripleConcat(name, _, Align)($this.addr, (*$this.addr)->len + 1);                \
		(*$this.addr)->buf[(*$this.addr)->len] = data;                                   \
		(*$this.addr)->len++;                                                            \
	}
