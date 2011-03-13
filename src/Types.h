#import "Macros.h"

/* Clang's stdbool.h is not used because it defines `true' and `false'
 * as plain integers, making it impossible to differentiate between
 * `int' and `bool' in function declarations, which can be necessary
 * when overloading functions.
 */

set(bool) {
	false = 0,
	true  = 1
};

#define false (bool) false
#define true  (bool) true

#ifndef NULL
#define NULL \
	((void *) 0)
#endif

/* Short names for explicitly-sized types. */
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

/* 16/32/64 bit big-endian representation. */
typedef u16 be16;
typedef u32 be32;
typedef u64 be64;

/* 16/32/64 bit little-endian representation. */
typedef u16 le16;
typedef u32 le32;
typedef u64 le64;

typedef void Generic;

typedef union {
	Generic *object;
} GenericInstance transparentUnion;

InstanceMethods(Generic);

/* Inline functions cannot be used as Pool's methods are not available yet. */
#define Generic_New(size) (GenericInstance) { .object = Pool_Alloc(Pool_GetInstance(), size) }
#define Generic_Free(instance) Pool_Free(Pool_GetInstance(), instance.object)

record(GenericCallback) {
	GenericInstance context;
	void (*cb)(GenericInstance context);
};

static inline GenericCallback Callback(GenericInstance context, void *cb) {
	return (GenericCallback) {
		.context = context,
		.cb      = cb
	};
}

static inline GenericCallback EmptyCallback(void) {
	return (GenericCallback) {
		.context = { NULL },
		.cb      = NULL
	};
}
