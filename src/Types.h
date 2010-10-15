#import "UniStd.h" /* size_t */

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
