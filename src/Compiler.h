#define likely(x) \
	__builtin_expect((x), 1)

#define unlikely(x) \
	__builtin_expect((x), 0)

#define __section(x) \
	__attribute__((__section__(x)))

#define __unused \
	__attribute__((unused))

#define alwaysInline \
	__attribute__((always_inline))

#define overload \
	__attribute__((overloadable))
