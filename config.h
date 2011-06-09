/* Save the file name and line number from which an exception was
 * raised.
 */
#define Exception_SaveOrigin 1

/* Clang only implements DWARF 2. Setting this to 3 would require
 * that all binaries used in the DWARF module implement the DWARF 3
 * specification.
 */
#define DWARF_Version 2

/* The DWARF module can only deal exclusively with 32-bit or 64-bit
 * binaries.
 */
#define DWARF_64bit defined(__x86_64__)

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
