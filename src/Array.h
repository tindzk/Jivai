#define Array_Sort(this, cmp) \
	qsort((this)->buf, (this)->len, sizeof((this)->buf[0]), (void *) cmp)

#define Array(type, name)                                                                      \
	record(name) {                                                                             \
		size_t len;                                                                            \
		type buf[];                                                                            \
	};                                                                                         \
	Instance(name);                                                                            \
	static inline name* tripleConcat(name, _, New)(size_t items) {                             \
		name *res = Pool_Alloc(Pool_GetInstance(), sizeof(name) + items * sizeof(type));       \
		res->len  = 0;                                                                         \
		return res;                                                                            \
	}                                                                                          \
	static inline size_t tripleConcat(name, _, GetSize)(InstName(name) $this) {                \
		return (Pool_GetSize(Pool_GetInstance(), $this.object) - sizeof(name)) / sizeof(type); \
	}                                                                                          \
	static inline void tripleConcat(name, _, Free)(InstName(name) $this) {                     \
		Pool_Free(Pool_GetInstance(), $this.object);                                           \
	}                                                                                          \
	static inline void tripleConcat(name, _, Delete)(InstName(name) $this, size_t idx) {       \
		if ($this.object->len - idx - 1 > 0) {                                                 \
			Memory_Copy(                                                                       \
				 $this.object->buf + idx,                                                      \
				 $this.object->buf + idx + 1,                                                  \
				($this.object->len - idx - 1) * sizeof(type));                                 \
		}                                                                                      \
		$this.object->len--;                                                                   \
	}                                                                                          \
	static inline void tripleConcat(name, _, Resize)                                           \
		(union { name **object; } transparentUnion $this, size_t items)                        \
	{                                                                                          \
		*$this.object = Pool_Realloc(Pool_GetInstance(), *$this.object,                        \
			sizeof(name) + items * sizeof(type));                                              \
		if ((*$this.object)->len > items) {                                                    \
			(*$this.object)->len = items;                                                      \
		}                                                                                      \
	}                                                                                          \
	static inline void tripleConcat(name, _, Align)                                            \
		(union { name **object; } transparentUnion $this, size_t items)                        \
	{                                                                                          \
		if (items > 0 && items > tripleConcat(name, _, GetSize)(*$this.object)) {              \
			tripleConcat(name, _, Resize)($this.object, items);                                \
		}                                                                                      \
	}                                                                                          \
	static inline void tripleConcat(name, _, Push)                                             \
		(union { name **object; } transparentUnion $this, type data)                           \
	{                                                                                          \
		tripleConcat(name, _, Align)($this.object, (*$this.object)->len + 1);                  \
		(*$this.object)->buf[(*$this.object)->len] = data;                                     \
		(*$this.object)->len++;                                                                \
	}
