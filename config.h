/* Save the file name and line number from which an exception was
 * raised.
 */
#define Exception_SaveOrigin 1

/* Enable assertions. */
#define Exception_Assert 1

/* Include a trace with 20 elements at most. */
#define Exception_SaveTrace  1
#define Exception_TraceSize 20

/* This will possibly improve the performance when dealing with
 * long strings.
 */
#define String_SmartAlign 1

/* Check for element mismatches in FmtString(). */
#define String_FmtChecks 1

/* Log everything by default. */
#define Logger_DisabledLevels 0
