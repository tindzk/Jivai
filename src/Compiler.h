#define likely(x) \
	__builtin_expect((x), 1)

#define unlikely(x) \
	__builtin_expect((x), 0)

#define __section(x) \
	__attribute__((__section__(x)))

#define __used \
	__attribute__((used))

#define __unused \
	__attribute__((unused))

#define noReturn \
	__attribute__((noreturn))

#define __malloc \
	__attribute__((malloc))

#define alwaysInline \
	__attribute__((always_inline))

#define transparentUnion \
	__attribute__((transparent_union))

#define overload \
	__attribute__((overloadable))

#define __constructor \
	__attribute__((constructor))

#define __destructor \
	__attribute__((destructor))
