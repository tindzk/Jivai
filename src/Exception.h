#import "Macros.h"

#ifndef Exception_SaveOrigin
#define Exception_SaveOrigin 1
#endif

#ifndef Exception_SaveTrace
#define Exception_SaveTrace 0
#endif

#ifndef Exception_Safety
#define Exception_Safety 0
#endif

#ifndef Exception_TraceSize
#define Exception_TraceSize 15
#endif

/* General exceptions. */
enum {
	excAccessDenied,
	excAlreadyExists,
	excAttributeNonExistent,
	excBufferTooSmall,
	excConnectionReset,
	excFcntlFailed,
	excGettingAttributeFailed,
	excIllegalNesting,
	excInvalidFileDescriptor,
	excInvalidParameter,
	excIsDirectory,
	excReadingFailed,
	excSettingAttributeFailed,
	excStatFailed,
	excTruncatingFailed,
	excUnknownError,

	excOffset
};

/* This structure contains more detailed information about the most
 * recently thrown exception. The exception itself is only supplied
 * in a numerical code via longjmp(). See ExceptionManager_Raise().
 */
typedef struct {
#if Exception_SaveOrigin
	String func;
#endif

	String scode;

#if Exception_SaveTrace
	void *trace[Exception_TraceSize];
	size_t traceItems;
#endif
} Exception;

void Exception_Print(Exception *e, size_t code);

#if Exception_SaveOrigin
#define Exception_SetOrigin(e) \
	(e).func = String(__func__)
#else
#define Exception_SetOrigin(e) \
	do { } while(0)
#endif

#if Exception_SaveTrace
#import "Backtrace.h"

#define Exception_SetTrace(e)            \
	(e).traceItems = Backtrace_GetTrace( \
		(e).trace,                       \
		Exception_TraceSize)
#else
#define Exception_SetTrace(e) \
	do { } while (0)
#endif

#define Exception_SetCode(e, c) \
	(e).scode = String(#c)
