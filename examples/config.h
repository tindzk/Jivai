/* Required when using Glibc. */
#define _LARGEFILE64_SOURCE

/* These checks will cause exceptions if certain parameters do not
 * make sense.
 */
#define Memory_BoundaryChecks
#define Memory_PointerChecks

/* Show an error in case you forgot to initialize a module's
 * exception manager.
 */
#define Exception_Safety

/* Save the file name and line number from which an exception was
 * raised.
 */
#define Exception_SaveOrigin

/* Include a trace with 20 elements at most. */
#define Exception_SaveTrace
#define Exception_TraceSize 20

/* Support formatting call traces. */
#define Backtrace_HasBFD

/* Log everything by default. */
#define Logger_DisabledLevels 0
