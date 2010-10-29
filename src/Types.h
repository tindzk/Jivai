#import "Macros.h"

/* Clang's stdbool.h is not used because it defines `true' and `false'
 * as plain integers, making it impossible to differentiate between
 * `int' and `bool' in function declarations, which can be necessary
 * when overloading functions.
 */

typedef enum {
	false = 0,
	true  = 1
} bool;

#define false (bool) false
#define true  (bool) true

#ifndef NULL
#define NULL \
	((void *) 0)
#endif

typedef   signed char       s8;
typedef unsigned char       u8;
typedef   signed short int s16;
typedef unsigned short int u16;
typedef   signed int       s32;
typedef unsigned int       u32;
typedef   signed long long s64;
typedef unsigned long long u64;

#if defined(__x86_64__)
	typedef unsigned long  size_t;
	typedef   signed long  ssize_t;
#else
	typedef unsigned int   size_t;
	typedef   signed int   ssize_t;
#endif

typedef void Generic;

typedef union {
	Generic *object;
} GenericInstance transparentUnion;

BasicInstance(Generic);

/* Inline functions cannot be used as Memory's methods are not available yet. */
#define Generic_New(size) (GenericInstance) { .object = Memory_Alloc(size) }
#define Generic_Free(instance) Memory_FreePtr(instance.object)

record(GenericCallback) {
	GenericInstance context;
	void *cb;
};

static inline GenericCallback Callback(GenericInstance context, void *cb) {
	return (GenericCallback) {
		.context = context,
		.cb      = cb
	};
}
