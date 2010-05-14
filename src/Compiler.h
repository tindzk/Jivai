#ifndef COMPILER_H
#define COMPILER_H

#define UNUSED \
	__attribute__ ((unused))

#define OVERLOAD \
	__attribute__((overloadable))

#define likely(x) \
	__builtin_expect((x), 1)

#define unlikely(x) \
	__builtin_expect((x), 0)

#endif
