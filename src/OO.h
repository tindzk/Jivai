#import "Types.h"
#import "Macros.h"
#import "Compiler.h"

// ---------
// Instances
// ---------

/* References any object. To be used in callbacks. */
typedef union {
	void *addr;
} Instance transparentUnion;

static alwaysInline bool Instance_IsValid(Instance $this) {
	return $this.addr != NULL;
}

#define InstName(name) \
	simpleConcat(name, Inst)

#define DynInstName(name) \
	simpleConcat(name, DynInst)

#define Instance(name)                    \
	typedef union {                       \
		name *addr;                       \
	} InstName(name) transparentUnion;    \
	typedef union {                       \
		name *addr;                       \
		Instance genericAddr;             \
	} DynInstName(name) transparentUnion; \

// -------
// Classes
// -------

/* Inspired by Nelson Elhage's NEWTYPE() macro.
 * http://blog.nelhage.com/2010/10/using-haskells-newtype-in-c/
 */
#define class                 \
	typedef struct self self; \
	Instance(self);           \
	struct self

#define this $this.addr

#define ref(name) \
	tripleConcat(self, _, name)

#define def(ret, method, ...) \
	ret ref(method)(__unused DynInstName(self) $this, ## __VA_ARGS__)

#define odef(ret, method, ...) \
	overload ret method(__unused InstName(self) $this, ## __VA_ARGS__)

#define rdef(ret, method, ...) \
	mustUseResult ret ref(method)(__unused DynInstName(self) $this, ## __VA_ARGS__)

#define sdef(ret, method, ...) \
	ret ref(method)(__VA_ARGS__)

#define rsdef(ret, method, ...) \
	mustUseResult ret ref(method)(__VA_ARGS__)

#define call(method, ...) \
	ref(method)($this, ## __VA_ARGS__)

#define scall(method, ...) \
	ref(method)(__VA_ARGS__)

// ------------------------
// Singleton Design Pattern
// ------------------------

#define SingletonPrototype(name) \
	name* simpleConcat(name, _GetInstance)(void)

#define Singleton(name, ...)                                \
	SingletonPrototype(name) {                              \
		static name object;                                 \
		static InstName(name) instance;                     \
		if (instance.addr == NULL) {                        \
			object = simpleConcat(name, _New)(__VA_ARGS__); \
			instance.addr = &object;                        \
		}                                                   \
		return instance.addr;                               \
	}

#define SingletonDestructor(name)               \
	Destructor {                                \
		self *instance =                        \
			simpleConcat(name, _GetInstance)(); \
		simpleConcat(name, _Destroy)(instance); \
	}

// ------------------------------
// Interfaces and Implementations
// ------------------------------

#define Interface(name)             \
	struct ItfName(name);           \
	record(name) {                  \
		struct ItfName(name) *impl; \
		Instance              inst; \
	};                              \
	record(ItfName(name))

#define ItfName(name) \
	simpleConcat(name, Interface)

#define ImplName(name) \
	simpleConcat(name, Impl)

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
				.addr = $this.addr      \
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

// ---------
// Callbacks
// ---------

#define Callback(name, ret, ...)                                \
	typedef ret simpleConcat(cb,name)                           \
		(Instance context, ## __VA_ARGS__);                     \
	typedef struct name {                                       \
		Instance context;                                       \
		simpleConcat(cb,name) *cb;                              \
	} name;                                                     \
	static alwaysInline name simpleConcat(name,_For)            \
		(Instance context, simpleConcat(cb,name) *cb) {         \
		return (name) { .context = context, .cb = cb };         \
	}                                                           \
	static alwaysInline name simpleConcat(name, _Empty)(void) { \
		return (name) { .cb = NULL };                           \
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
