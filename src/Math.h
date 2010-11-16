#import "Macros.h"

#define Abs(val) ({               \
	typeof(val) __val = (val);    \
	(__val < 0) ? -__val : __val; \
})

#define Ceil(val) ({               \
	typeof(val) __orig = Abs(val); \
	int __trunc = (int) __orig;    \
	(__orig - __trunc) > 0         \
		? __trunc + 1              \
		: __trunc;                 \
})

/* Clamp() and ClampValue() were taken from Linux kernel 2.6.35. */

/*
 * Return a value clamped to a given range with strict type
 * checking.
 *
 * This macro does strict type checking of `min' and `max' to make
 * sure they are of the same type as `val'.
 */
#define Clamp(val, min, max) ({                  \
	typeof(val) __val = (val);                   \
	typeof(min) __min = (min);                   \
	typeof(max) __max = (max);                   \
	staticAssert(                                \
		isType(typeof(&__val), typeof(&__min))); \
	staticAssert(                                \
		isType(typeof(&__val), typeof(&__max))); \
	__val = __val < __min ? __min : __val;       \
	__val > __max ? __max : __val;               \
})

/**
 * Return a value clamped to a given range using val's type.
 *
 * This macro does no type checking and uses temporary variables of
 * whatever type the input argument 'val' is.  This is useful when
 * `val' is an unsigned type and `min' and `max' are literals that
 * will otherwise be assigned a signed integer type.
 */
#define ClampValue(val, min, max) ({       \
	typeof(val) __val = (val);             \
	typeof(val) __min = (min);             \
	typeof(val) __max = (max);             \
	__val = __val < __min ? __min : __val; \
	__val > __max ? __max : __val;         \
})
