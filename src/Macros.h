#import "Compiler.h"

#define _quote(x) #x
#define quote(x)  _quote(x)

#define set(name)           \
	typedef enum name name; \
	enum name

#define record(name)          \
	typedef struct name name; \
	struct name

#define variant(name)        \
	typedef union name name; \
	union name

#define when(lbl) \
	if (0) lbl: if (1)

/* Hide Clang warning "expression result unused" (-Wunused-value). */
static alwaysInline int _useValue(int value) {
	return value;
}

/* ret() can be used to leave functions with little effort:
 *  1. expr || ret(false);
 *  2. while (expr || ret(false)) { ... }
 *
 * As opposed to:
 *  1. if (!expr) {
 *         return false;
 *     }
 *  2. while (true) {
 *         if (!expr) {
 *             return false;
 *         }
 *         ...
 *     }
 */
#define ret(val) \
	_useValue(({ return val; false; }))

#define jmp(lbl) \
	_useValue(({ goto lbl; false; }))

#define range(i, lower, upper) \
	for (typeof(lower) i = (lower); i <= (upper); i++)

#define rpt(n) \
	for (size_t __rpt_i = (n); __rpt_i; __rpt_i--)

#define fwd(i, cnt) \
	for (typeof(cnt) i = 0; i < (cnt); i++)

#define bwd(i, cnt) \
	if (cnt) for (typeof(cnt) i = (cnt); i--;)

#define _simpleConcat(x, y) \
	x ## y

#define simpleConcat(x, y) \
	_simpleConcat(x, y)

#define _tripleConcat(x, y, z) \
	x ## y ## z

#define tripleConcat(x, y, z) \
	_tripleConcat(x, y, z)

#define Constructor \
	static void __constructor ctor(void)

#define Destructor \
	static void __destructor dtor(void)

/* Iterate over arrays and tree nodes using pointer arithmetic. As
 * we don't declare an index variable, this macro can be nested
 * safely.
 */
#define each(name, arr)                               \
	for (                                             \
		typeof((arr)->buf[0]) *name = &(arr)->buf[0]; \
		name < &(arr)->buf[(arr)->len];               \
		name++)

#define seach(i, name, arr)                                  \
	for (typeof((arr)->len) i = 0; ; ({ break; }))           \
	for (typeof((arr)->buf[0]) name;                         \
		i < (arr)->len && ({ name = (arr)->buf[i]; true; }); \
		i++)

#define getIndex(name, arr) \
	(size_t) ((name) - (arr)->buf)

#define isEven(name, arr) \
	(getIndex(name, arr) % 2) == 0

#define isLast(name, arr) \
	((name) + 1 == (arr)->buf + (arr)->len)

/* staticAssertValue(), mustBeArray() and nElems() are taken from
 * Linux kernel 2.6.35.
 */

/* See tools/perf/util/include/linux/kernel.h */
#define staticAssertValue(expr) \
	(sizeof(struct { int:-!(expr); }))

#define staticAssert(expr) \
	(void) staticAssertValue(expr)

/* See include/linux/compiler-gcc.h
 * &a[0] degrades to a pointer: a different type from an array
 */
#define mustBeArray(arr) \
	staticAssertValue(!isType(typeof(arr), typeof(&arr[0])))

/* See include/linux/kernel.h */
#define nElems(arr) \
	(sizeof(arr) / sizeof((arr)[0]) + mustBeArray(arr))

/* The following macros were taken from:
 * http://www.fefe.de/intof.html
 * http://coverclock.blogspot.com/2008/05/generic-embedded-programming-using-c.html
 */

#define MinUnsigned(type) \
	((type) 0)

#define MaxUnsigned(type) \
	((type) (~MinUnsigned(type)))

#define HalfMaxSigned(type) \
	((type) 1 << (sizeof(type) * 8 - 2))

#define MaxSigned(type) \
	(HalfMaxSigned(type) - 1 + HalfMaxSigned(type))

#define MinSigned(type) \
	(-1 - MaxSigned(type))

#define isSigned(type) \
	((type) -1 < 1)

#define MinValue(type) (    \
	isSigned(type)          \
		? MinSigned(type)   \
		: MinUnsigned(type) \
)

#define MaxValue(type) (    \
	isSigned(type)          \
		? MaxSigned(type)   \
		: MaxUnsigned(type) \
)

#define safeAssign(dest, src) ({        \
	typeof(src)  _x = (src);            \
	typeof(dest) _y = _x;               \
	(_x == _y && ((_x < 1) == (_y < 1)) \
		? (void) ((dest) = _y), 1       \
		: 0);                           \
})

#define safeAdd(c, a, b) ({                 \
	typeof(a) _a = a;                       \
	typeof(b) _b = b;                       \
	(_b < 1)                                \
		? ((MinValue(typeof(c)) - _b <= _a) \
			? safeAssign(c, _a + _b)        \
			: 0)                            \
		: ((MaxValue(typeof(c)) - _b >= _a) \
			? safeAssign(c, _a + _b)        \
			: 0);                           \
})
