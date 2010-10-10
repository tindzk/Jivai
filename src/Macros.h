#define __paste(x, y) \
	x ## _ ## y

#define __eval(x, y) \
	__paste(x, y)

#define ref(name) \
	__eval(self, name)

#define def(ret, name, ...) \
	ret ref(name)(self *restrict this, ## __VA_ARGS__)

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
