#import <setjmp.h>

#import "Macros.h"

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
	int traceItems;
#endif
} Exception;

typedef struct _ExceptionManager_Record {
	struct _ExceptionManager_Record *prev; /* linked list */
	jmp_buf jmpBuffer;
} ExceptionManager_Record;

typedef struct {
	ExceptionManager_Record *cur;
	Exception e;
} ExceptionManager;

#define Exception_Entry(name) \
	[name] = String(#name)
