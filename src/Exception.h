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

typedef struct {
#if Exception_SaveOrigin
	String func;
#endif

	size_t module;
	size_t code;

	String scode;

#if Exception_SaveTrace
	void *trace[Exception_TraceSize];
	size_t traceItems;
#endif
} Exception;

void Exception_Print(Exception *e);

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
	do {                        \
		(e).code  = c;          \
		(e).scode = String(#c); \
	} while(0)

#define Exception_SetModule(e) \
	(e).module = __eval(Modules, self)
