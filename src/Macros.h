/* Currently local labels are not supported by Clang.
 *
 * Therefore there cannot be more than one exception
 * block per function.
 *
 * See also http://llvm.org/bugs/show_bug.cgi?id=3429
 */
#define __label__ \
	__unused void *

#define set(name)           \
	typedef enum name name; \
	enum name

#define record(name)          \
	typedef struct name name; \
	struct name

#define when(lbl) \
	if (0) lbl: if (1)

#define repeat(n) \
	for (size_t i = (n); i; i--)

#define _simpleConcat(x, y) \
	x ## y

#define simpleConcat(x, y) \
	_simpleConcat(x, y)

#define _underscoredConcat(x, y) \
	x ## _ ## y

#define underscoredConcat(x, y) \
	_underscoredConcat(x, y)

#define $(s) String(s)

#define NullString \
	(String) { 0, 0, NULL, false }

#define ref(name) \
	underscoredConcat(self, name)

#define call(method, ...) \
	ref(method)($this, ## __VA_ARGS__)

#define CurModule \
	underscoredConcat(Modules, self)

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

#define class(name)                        \
	typedef struct name name;              \
	typedef union {                        \
		struct name *object;               \
		GenericInstance generic;           \
	} InstanceName(name) transparentUnion; \
	BasicInstance(name)                    \
	struct name

/* This cannot be included in Class() as sizeof() is needed and the
 * structure's final size is not yet determinable.
 */
#define ExtendClass(name)                                                                     \
	static __unused alwaysInline InstanceName(name) underscoredConcat(name, NewStack)(void) { \
		name obj;                                                                             \
		return (InstanceName(name)) &obj;                                                     \
	}                                                                                         \
	static inline InstanceName(name) underscoredConcat(name, New)(void) {                     \
		return (InstanceName(name)) (name *) Memory_Alloc(sizeof(name));                               \
	}                                                                                         \
	static inline void underscoredConcat(name, Free)(InstanceName(name) instance) {           \
		Memory_Free(instance.object);                                                         \
	}

#define SingletonPrototype(name) \
	InstanceName(name) underscoredConcat(name, GetInstance)(void);

#define Singleton(name, ...)                                         \
	InstanceName(name) underscoredConcat(name, GetInstance)(void) {  \
		static name object;                                          \
		static InstanceName(name) instance;                          \
		if (underscoredConcat(name, IsNull)(instance)) {             \
			instance = underscoredConcat(name, FromObject)(&object); \
			underscoredConcat(name, Init)(instance, ## __VA_ARGS__); \
		}                                                            \
		return instance;                                             \
	}

#define SingletonDestructor(name)                   \
	static void __destructor dtor(void) {           \
		InstanceName(self) instance =               \
			underscoredConcat(name, GetInstance)(); \
		underscoredConcat(name, Destroy)(instance); \
	}

#define DefineCallback(name, ret, ...)                          \
	typedef union name {                                        \
		GenericCallback generic;                                \
		struct {                                                \
			GenericInstance context;                            \
			ret (*cb)(GenericInstance context, ## __VA_ARGS__); \
		};                                                      \
	} name transparentUnion;

#define callback(var, ...) \
	(var).cb((var).context, ## __VA_ARGS__)

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
