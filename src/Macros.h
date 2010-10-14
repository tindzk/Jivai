#define when(lbl) if (0) lbl: if (1)

#define _simpleConcat(x, y) \
	x ## y

#define simpleConcat(x, y) \
	_simpleConcat(x, y)

#define _underscoredConcat(x, y) \
	x ## _ ## y

#define underscoredConcat(x, y) \
	_underscoredConcat(x, y)

#define ref(name) \
	underscoredConcat(self, name)

#define call(method, ...) \
	ref(method)($this, ## __VA_ARGS__)

#define ClassName(name) \
	simpleConcat(name, Class)

#define def(ret, method, ...) \
	ret ref(method)(__unused ClassName(self) $this, ## __VA_ARGS__)

#define BasicClass(name)                                                         \
	static inline ClassName(name) underscoredConcat(name, AsClass)(name *ptr) {  \
		return (ClassName(name)) { .p = ptr };                                   \
	}                                                                            \
	static inline name* underscoredConcat(name, GetPtr)(ClassName(name) class) { \
		return class.p;                                                          \
	}                                                                            \
	static inline ClassName(name) underscoredConcat(name, Null)() {              \
		return (ClassName(name)) { .p = NULL };                                  \
	}                                                                            \
	static inline bool underscoredConcat(name, IsNull)(ClassName(name) class) {  \
		return class.p == NULL;                                                  \
	}                                                                            \
	static inline bool underscoredConcat(name, Equals)(ClassName(name) a, ClassName(name) b) { \
		return a.p == b.p;                                                       \
	}

/* Inspired by Nelson Elhage's NEWTYPE() macro.
 * http://blog.nelhage.com/2010/10/using-haskells-newtype-in-c/
 */

#define Class(name)                                                              \
	name;                                                                        \
	typedef struct {                                                             \
		name *p;                                                                 \
	} simpleConcat(name, Class);                                                 \
	BasicClass(name)                                                             \
	static inline ClassName(name) underscoredConcat(name, New)(void) {           \
		return (ClassName(name)) { .p = Memory_Alloc(sizeof(name)) };            \
	}                                                                            \
	static inline void underscoredConcat(name, Free)(ClassName(name) class) {    \
		Memory_Free(class.p);                                                    \
	}

typedef void Generic;

typedef struct {
	void *p;
} GenericClass;

BasicClass(Generic)

/* Inline functions cannot be used as Memory's methods are not available yet. */
#define Generic_New(size) (GenericClass) { .p = Memory_Alloc(size) }
#define Generic_Free(class) ({ void *ptr = class.p; Memory_Free(ptr); })

/* buildBugOnZero(), mustBeArray() and nElems() are taken from Linux
 * kernel 2.6.35.
 */

/* See tools/perf/util/include/linux/kernel.h */
#define buildBugOnZero(e) \
	(sizeof(struct { int:-!!(e); }))

/* See include/linux/compiler-gcc.h
 * &a[0] degrades to a pointer: a different type from an array
 */
#define mustBeArray(a) \
	buildBugOnZero(__builtin_types_compatible_p(typeof(a), typeof(&a[0])))

/* See include/linux/kernel.h */
#define nElems(arr) \
	(sizeof(arr) / sizeof((arr)[0]) + mustBeArray(arr))
