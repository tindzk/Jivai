/* These checks will cause exceptions if certain parameters do not
 * make sense.
 */
#define Memory_BoundaryChecks    1
#define Memory_PointerChecks     1
#define Memory_OutOfMemoryChecks 0

/* Show an error in case you forgot to initialize a module's
 * exception manager.
 */
#define Exception_Safety 1

/* Save the file name and line number from which an exception was
 * raised.
 */
#define Exception_SaveOrigin 1

/* Include a trace with 20 elements at most. */
#define Exception_SaveTrace  0
#define Exception_TraceSize 20

/* This will possibly improve the performance when dealing with
 * long strings.
 */
#define String_SmartAlign 1

/* Support formatting call traces. */
#define Backtrace_HasBFD

/* Log everything by default. */
#define Logger_DisabledLevels 0

#import "Manifest.h"
