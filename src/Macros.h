#import "Compiler.h"

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

#define range(i, lower, upper) \
	for (typeof(lower) i = (lower); i <= (upper); i++)

#define repeat(n) \
	for (size_t __repeat_i = (n); __repeat_i; __repeat_i--)

#define forward(i, cnt) \
	for (typeof(cnt) i = 0; i < (cnt); i++)

#define reverse(i, cnt) \
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

#define Method(ret, method, ...) \
	ret (*method)(GenericInstance $this, ## __VA_ARGS__)

#define Instance(name) \
	simpleConcat(name, Instance)

#define Interface(name)               \
	struct name##Interface;           \
	record(name) {                    \
		struct name##Interface *impl; \
		GenericInstance         inst; \
	};                                \
	record(name##Interface)

#define shareSize \
	.size = sizeof(self)

#define share(src, dest) \
	.dest = (void *) ref(src)

#define delegate(name, method, ...) \
	(name).impl->method((name).inst, ## __VA_ARGS__)

#define implements(name, method) \
	(name).impl->method != NULL

#define ImplName \
	simpleConcat(self, Impl)

#define Impl(name) \
	name##Interface ImplName

#define DefineAs(name, implName)        \
	static inline def(name, As##name) { \
		return (name) {                 \
			.impl = &implName,          \
			.inst = {                   \
				.object = $this.object  \
			}                           \
		};                              \
	}

#define ExportImpl(name)                             \
	extern Impl(name);                               \
	static inline sdef(name##Interface *, GetImpl) { \
		return &ImplName;                            \
	}                                                \
	DefineAs(name, ImplName);

#define ExportAnonImpl(module, name)                 \
	name##Interface simpleConcat(module, Impl);      \
	static inline sdef(name##Interface *, GetImpl) { \
		return &simpleConcat(module, Impl);          \
	}

#define ImplExName(name) \
	simpleConcat(self, _##name##Impl)

#define ImplEx(name) \
	name##Interface ImplExName(name)

#define ExportAnonImplEx(module, name) \
	name##Interface simpleConcat(module, _##name##Impl)

#define ExportImplEx(name) \
	extern ImplEx(name);   \
	DefineAs(name, ImplExName(name));

#define def(ret, method, ...) \
	ret ref(method)(__unused Instance(self) $this, ## __VA_ARGS__)

#define sdef(ret, method, ...) \
	ret ref(method)(__VA_ARGS__)

#define this $this.object

#define Constructor \
	static void __constructor ctor(void)

#define Destructor \
	static void __destructor dtor(void)

/* Iterate over arrays and tree nodes using pointer arithmetic. As
 * we don't declare an index variable, this macro can be nested
 * safely.
 */
#define foreach(name, arr)                            \
	for (                                             \
		typeof((arr)->buf[0]) *name = &(arr)->buf[0]; \
		name < &(arr)->buf[(arr)->len];               \
		name++)

#define getIndex(name, arr) \
	(size_t) ((arr)->buf + (arr)->len - (name) - 1)

#define isLast(name, arr) \
	((name) + 1 == (arr)->buf + (arr)->len)

/* Inspired by Nelson Elhage's NEWTYPE() macro.
 * http://blog.nelhage.com/2010/10/using-haskells-newtype-in-c/
 */

#define BasicInstance(name)                                                                   \
	static inline Instance(name) tripleConcat(name, _, FromObject)(name *object) {            \
		return (Instance(name)) { .object = object };                                         \
	}                                                                                         \
	static inline name* tripleConcat(name, _, GetObject)(Instance(name) instance) {           \
		return instance.object;                                                               \
	}                                                                                         \
	static inline Instance(name) tripleConcat(name, _, Null)() {                              \
		return (Instance(name)) { .object = NULL };                                           \
	}                                                                                         \
	static inline bool tripleConcat(name, _, IsNull)(Instance(name) instance) {               \
		return instance.object == NULL;                                                       \
	}                                                                                         \
	static inline GenericInstance tripleConcat(name, _, ToGeneric)(Instance(name) instance) { \
		return (GenericInstance) { .object = instance.object };                               \
	}

#define class                          \
	typedef struct self self;          \
	typedef union {                    \
		struct self *object;           \
		GenericInstance generic;       \
	} Instance(self) transparentUnion; \
	BasicInstance(self)                \
	struct self

/* This cannot be included in `class' as sizeof() is needed and the
 * structure's final size is not yet determinable.
 */
#define ExtendClass                                                                        \
	static inline bool tripleConcat(self, _, Equals)(Instance(self) a, Instance(self) b) { \
		return a.object == b.object;                                                       \
	}                                                                                      \
	static alwaysInline Instance(self) tripleConcat(self, _, NewStack)(void) {             \
		self obj;                                                                          \
		return (Instance(self)) &obj;                                                      \
	}                                                                                      \
	static inline Instance(self) tripleConcat(self, _, New)(void) {                        \
		return (Instance(self)) (self *) Memory_Alloc(sizeof(self));                       \
	}                                                                                      \
	static inline void tripleConcat(self, _, Free)(Instance(self) instance) {              \
		Memory_Free(instance.object);                                                      \
	}                                                                                      \
	static inline Instance(self) tripleConcat(self, _, Clone)(Instance(self) instance) {   \
		return (Instance(self)) (self *) Memory_Clone(instance.object, sizeof(self));      \
	}

#define SingletonPrototype(name) \
	Instance(name) tripleConcat(name, _, GetInstance)(void)

#define Singleton(name, ...)                                       \
	SingletonPrototype(name) {                                     \
		static name object;                                        \
		static Instance(name) instance;                            \
		if (tripleConcat(name, _, IsNull)(instance)) {             \
			instance = tripleConcat(name, _, FromObject)(&object); \
			tripleConcat(name, _, Init)(instance, ## __VA_ARGS__); \
		}                                                          \
		return instance;                                           \
	}

#define SingletonDestructor(name)                 \
	Destructor {                                  \
		Instance(self) instance =                 \
			tripleConcat(name, _, GetInstance)(); \
		tripleConcat(name, _, Destroy)(instance); \
	}

#define DefineCallback(name, ret, ...)                          \
	typedef union name {                                        \
		GenericCallback generic;                                \
		struct {                                                \
			GenericInstance context;                            \
			ret (*cb)(GenericInstance context, ## __VA_ARGS__); \
		};                                                      \
	} name transparentUnion;

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
