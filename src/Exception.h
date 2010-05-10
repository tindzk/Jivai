#ifndef EXCEPTION_H
#define EXCEPTION_H

#ifndef Exception_TraceSize
#define Exception_TraceSize 15
#endif

#include <setjmp.h>

typedef struct {
#ifdef Exception_SaveOrigin
	String file;
	int line;
#endif

	void *p;

#ifdef Exception_SaveTrace
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
	String name = StaticString(#name)

#define Exception_Export(name) \
	extern String name

#endif
