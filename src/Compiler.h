#define likely(x) \
	__builtin_expect((x), 1)

#define unlikely(x) \
	__builtin_expect((x), 0)

#define nonNull(x) \
	__attribute__((nonnull(x)))

#define __section(x) \
	__attribute__((__section__(x)))

#define __packed \
	__attribute__((__packed__))

#define __used \
	__attribute__((used))

#define __unused \
	__attribute__((unused))

#define noReturn \
	__attribute__((noreturn))

#define mustUseResult \
	__attribute__((warn_unused_result))

#define __malloc \
	__attribute__((malloc))

#define alwaysInline \
	__attribute__((unused, always_inline))

#define transparentUnion \
	__attribute__((transparent_union))

#define override \
	__attribute__((weak))

#define overload \
	__attribute__((overloadable))

/* TODO This doesn't work yet in Clang,
 * see http://llvm.org/bugs/show_bug.cgi?id=5329 and
 *     http://llvm.org/bugs/show_bug.cgi?id=9878
 */
#define earlyConstructor \
	__attribute__((constructor(100)))

#define __constructor \
	__attribute__((constructor(101)))

#define __destructor \
	__attribute__((destructor))

#define isType(a, b) \
	__builtin_types_compatible_p(a, b)

#define VarArg_Start(v, l) \
	__builtin_va_start((v), (l))

#define VarArg_End \
	__builtin_va_end

#define VarArg_Get \
	__builtin_va_arg

#define VarArg_Copy(d, s) \
	__builtin_va_copy((d), (s))

typedef __builtin_va_list VarArg;
