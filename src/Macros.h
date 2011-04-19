#import "Compiler.h"

#define set(name)           \
	typedef enum name name; \
	enum name

#define record(name)          \
	typedef struct name name; \
	struct name

#define when(lbl) \
	if (0) lbl: if (1)

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

#define ref(name) \
	tripleConcat(self, _, name)

#define call(method, ...) \
	ref(method)($this, ## __VA_ARGS__)

#define scall(method, ...) \
	ref(method)(__VA_ARGS__)

#define ItfName(name) \
	simpleConcat(name, Interface)

#define InstName(name) \
	simpleConcat(name, Instance)

#define ExtendedInstName(name) \
	simpleConcat(name, ExtendedInstance)

#define ImplName(name) \
	simpleConcat(name, Impl)

#define Instance(name)                        \
	typedef union {                           \
		name *object;                         \
	} InstName(name) transparentUnion;        \
	typedef union {                           \
		name *object;                         \
		GenericInstance generic;              \
	} ExtendedInstName(name) transparentUnion; \

#define Interface(name)             \
	struct ItfName(name);           \
	record(name) {                  \
		struct ItfName(name) *impl; \
		GenericInstance      inst;  \
	};                              \
	record(ItfName(name))

#define delegate(name, method, ...) \
	(name).impl->method((name).inst, ## __VA_ARGS__)

#define implements(name, method) \
	(name).impl->method != NULL

#define Impl(name) \
	ItfName(name) ImplName(self)

#define DefineAs(name, implName)        \
	static inline def(name, As##name) { \
		return (name) {                 \
			.impl = &implName,          \
			.inst = {                   \
				.object = $this.object  \
			}                           \
		};                              \
	}

#define ExportImpl(name)                           \
	extern Impl(name);                             \
	static inline sdef(ItfName(name) *, GetImpl) { \
		return &ImplName(self);                    \
	}                                              \
	DefineAs(name, ImplName(self));

#define ExportAnonImpl(module, name)               \
	ItfName(name) simpleConcat(module, Impl);      \
	static inline sdef(ItfName(name) *, GetImpl) { \
		return &simpleConcat(module, Impl);        \
	}

#define ImplExName(name) \
	simpleConcat(self, _##name##Impl)

#define ImplEx(name) \
	ItfName(name) ImplExName(name)

#define ExportAnonImplEx(module, name) \
	ItfName(name) simpleConcat(module, _##name##Impl)

#define ExportImplEx(name) \
	extern ImplEx(name);   \
	DefineAs(name, ImplExName(name));

#define def(ret, method, ...) \
	ret ref(method)(__unused ExtendedInstName(self) $this, ## __VA_ARGS__)

#define odef(ret, method, ...) \
	overload ret method(__unused InstName(self) $this, ## __VA_ARGS__)

#define rdef(ret, method, ...) \
	mustUseResult ret ref(method)(__unused ExtendedInstName(self) $this, ## __VA_ARGS__)

#define sdef(ret, method, ...) \
	ret ref(method)(__VA_ARGS__)

#define rsdef(ret, method, ...) \
	mustUseResult ret ref(method)(__VA_ARGS__)

#define this $this.object

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

#define getIndex(name, arr) \
	(size_t) ((name) - (arr)->buf)

#define isEven(name, arr) \
	(getIndex(name, arr) % 2) == 0

#define isLast(name, arr) \
	((name) + 1 == (arr)->buf + (arr)->len)

/* Inspired by Nelson Elhage's NEWTYPE() macro.
 * http://blog.nelhage.com/2010/10/using-haskells-newtype-in-c/
 */

#define InstanceMethods(name)                                                                 \
	static inline InstName(name) tripleConcat(name, _, FromObject)(name *object) {            \
		return (InstName(name)) { .object = object };                                         \
	}                                                                                         \
	static inline name* tripleConcat(name, _, GetObject)(InstName(name) instance) {           \
		return instance.object;                                                               \
	}                                                                                         \
	static inline InstName(name) tripleConcat(name, _, Null)() {                              \
		return (InstName(name)) { .object = NULL };                                           \
	}                                                                                         \
	static inline bool tripleConcat(name, _, IsNull)(InstName(name) instance) {               \
		return instance.object == NULL;                                                       \
	}                                                                                         \
	static inline GenericInstance tripleConcat(name, _, ToGeneric)(InstName(name) instance) { \
		return (GenericInstance) { .object = instance.object };                               \
	}                                                                                         \
	static inline bool tripleConcat(name, _, Is)(InstName(name) a, InstName(name) b) {        \
		return a.object == b.object;                                                          \
	}                                                                                         \

#define class                 \
	typedef struct self self; \
	Instance(self);           \
	InstanceMethods(self);    \
	struct self

/* This cannot be included in `class' as sizeof() is needed and the
 * structure's final size is not yet determinable.
 */
#define MemoryHelpers(name)                                           \
	static inline name* tripleConcat(name, _, Alloc)(void) {          \
		return (name *) Pool_Alloc(Pool_GetInstance(), sizeof(name)); \
	}                                                                 \
	static inline void tripleConcat(name, _, Free)(name *ptr) {       \
		Pool_Free(Pool_GetInstance(), ptr);                           \
	}                                                                 \
	static inline name* tripleConcat(name, _, Clone)(name *src) {     \
		name *dest = Pool_Alloc(Pool_GetInstance(), sizeof(name));    \
		Memory_Move(dest, src, sizeof(name));                         \
		return dest;                                                  \
	}

#define SingletonPrototype(name) \
	name* tripleConcat(name, _, GetInstance)(void)

#define Singleton(name, ...)                                       \
	SingletonPrototype(name) {                                     \
		static name object;                                        \
		static InstName(name) instance;                            \
		if (tripleConcat(name, _, IsNull)(instance)) {             \
			object   = tripleConcat(name, _, New)(__VA_ARGS__);    \
			instance = tripleConcat(name, _, FromObject)(&object); \
		}                                                          \
		return instance.object;                                    \
	}

#define SingletonEx(name, ...)                                     \
	SingletonPrototype(name) {                                     \
		static name object;                                        \
		static InstName(name) instance;                            \
		if (tripleConcat(name, _, IsNull)(instance)) {             \
			tripleConcat(name, _, Init)(&object);                  \
			instance = tripleConcat(name, _, FromObject)(&object); \
		}                                                          \
		return instance.object;                                    \
	}

#define SingletonDestructor(name)                 \
	Destructor {                                  \
		self *instance =                          \
			tripleConcat(name, _, GetInstance)(); \
		tripleConcat(name, _, Destroy)(instance); \
	}

#define Callback(name, ret, ...)                               \
	typedef ret simpleConcat(cb,name)                          \
		(GenericInstance context, ## __VA_ARGS__);             \
	typedef struct name {                                      \
		GenericInstance context;                               \
		simpleConcat(cb,name) *cb;                             \
	} name;                                                    \
	static alwaysInline name simpleConcat(name,_For)           \
		(GenericInstance context, simpleConcat(cb,name) *cb) { \
		return (name) { .context = context, .cb = cb };        \
	}                                                          \
	static alwaysInline name simpleConcat(name,_Empty)(void) { \
		return (name) { .cb = NULL };                          \
	}

#define hasCallback(var) \
	((var).cb != NULL)

#define _callback(var, ...) \
	(var).cb((var).context, ## __VA_ARGS__)

#define callback(var, ...)              \
	if (hasCallback(var)) {             \
		_callback(var, ## __VA_ARGS__); \
	}

#define callbackRet(var, default, ...) ( \
	hasCallback(var)                     \
		? _callback(var, ## __VA_ARGS__) \
		: default                        \
)

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
