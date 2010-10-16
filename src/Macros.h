#define set(name)           \
	typedef enum name name; \
	enum name

#define record(name)          \
	typedef struct name name; \
	struct name

#define when(lbl) if (0) lbl: if (1)

#define _simpleConcat(x, y) \
	x ## y

#define simpleConcat(x, y) \
	_simpleConcat(x, y)

#define _underscoredConcat(x, y) \
	x ## _ ## y

#define underscoredConcat(x, y) \
	_underscoredConcat(x, y)

#define $(s) String(s)

#define ref(name) \
	underscoredConcat(self, name)

#define call(method, ...) \
	ref(method)($this, ## __VA_ARGS__)

#define InstanceName(name) \
	simpleConcat(name, Instance)

#define def(ret, method, ...) \
	ret ref(method)(__unused InstanceName(self) $this, ## __VA_ARGS__)

/* Inspired by Nelson Elhage's NEWTYPE() macro.
 * http://blog.nelhage.com/2010/10/using-haskells-newtype-in-c/
 */

#define BasicInstance(name)                                                               \
	static inline InstanceName(name) underscoredConcat(name, FromObject)(name *object) {  \
		return (InstanceName(name)) { .object = object };                                 \
	}                                                                                     \
	static inline name* underscoredConcat(name, GetObject)(InstanceName(name) instance) { \
		return instance.object;                                                           \
	}                                                                                     \
	static inline InstanceName(name) underscoredConcat(name, Null)() {                    \
		return (InstanceName(name)) { .object = NULL };                                   \
	}                                                                                     \
	static inline bool underscoredConcat(name, IsNull)(InstanceName(name) instance) {     \
		return instance.object == NULL;                                                   \
	}                                                                                     \
	static inline bool underscoredConcat(name, Equals)(InstanceName(name) a, InstanceName(name) b) { \
		return a.object == b.object;                                                      \
	}                                                                                     \
	static inline GenericInstance underscoredConcat(name, ToGeneric)(InstanceName(name) instance) { \
		return (GenericInstance) { .object = instance.object };                           \
	}

#define Class(name)           \
	typedef struct name name; \
	typedef struct {          \
		struct name *object;  \
	} InstanceName(name);     \
	BasicInstance(name)       \
	struct name

/* This cannot be included in Class() as sizeof() is needed and the
 * structure's final size is not yet determinable.
 */
#define ExtendClass(name)                                                           \
	static inline InstanceName(name) underscoredConcat(name, New)(void) {           \
		return (InstanceName(name)) { .object = Memory_Alloc(sizeof(name)) };       \
	}                                                                               \
	static inline void underscoredConcat(name, Free)(InstanceName(name) instance) { \
		Memory_Free(instance.object);                                               \
	}

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
