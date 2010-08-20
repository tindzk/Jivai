#define UNUSED \
	__attribute__ ((unused))

#define OVERLOAD \
	__attribute__((overloadable))

#define __section(x) \
	__attribute__((__section__(x)))

#define likely(x) \
	__builtin_expect((x), 1)

#define unlikely(x) \
	__builtin_expect((x), 0)
