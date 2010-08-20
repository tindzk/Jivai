#import <setjmp.h>

#ifndef Exception_TraceSize
#define Exception_TraceSize 15
#endif

typedef struct {
#if Exception_SaveOrigin
	String file;
	int line;
#endif

	void *p;

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

#define Exception_Define(name) \
	String name __section(".exceptions") = String(#name)

#define Exception_Export(name) \
	extern String name
