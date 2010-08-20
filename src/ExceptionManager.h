#import "String.h"
#import "Integer.h"
#import "NULL.h"
#import "Backtrace.h"
#import "Compiler.h"

void ExceptionManager_Init(ExceptionManager *this);
void ExceptionManager_Raise(ExceptionManager *this);
void ExceptionManager_Push(ExceptionManager *this, ExceptionManager_Record *record);
void Exception_Print(Exception *e);

#ifndef Exception_SaveOrigin
#define Exception_SaveOrigin 1
#endif

#ifndef Exception_SaveTrace
#define Exception_SaveTrace 0
#endif

#ifndef Exception_Safety
#define Exception_Safety 0
#endif

#if Exception_Safety
#define ExceptionManager_Check(this)                        \
	do {                                                    \
		if ((this) == NULL) {                               \
			String_Print(String(                            \
				"The exception manager in "));              \
			String_Print(String(__FILE__));                 \
			String_Print(String(" is not initialized!\n")); \
			exit(EXIT_FAILURE);                             \
		}                                                   \
	} while(0)
#else
#define ExceptionManager_Check(...) \
	do { } while (0)
#endif

#if Exception_SaveOrigin
#define ExceptionManager_SetOrigin(this)   \
	do {                                   \
		(this)->e.file = String(__FILE__); \
		(this)->e.line = __LINE__;         \
	} while(0)
#else
#define ExceptionManager_SetOrigin(this) \
	do { } while(0)
#endif

#if Exception_SaveTrace
#define ExceptionManager_SetTrace(this)        \
	(this)->e.traceItems = Backtrace_GetTrace( \
		(this)->e.trace,                       \
		Exception_TraceSize)
#else
#define ExceptionManager_SetTrace(this) \
	do { } while (0)
#endif

#define ExceptionManager_Throw(this, c)    \
	do {                                   \
		ExceptionManager_Check(this);      \
		(this)->e.p = (void *) c;          \
		ExceptionManager_SetOrigin(this);  \
		ExceptionManager_SetTrace(this);   \
		ExceptionManager_Raise(this);      \
	} while(0)

#define ExceptionManager_Pop(this)           \
	do {                                     \
		if ((this)->cur != NULL) {           \
			(this)->cur = (this)->cur->prev; \
		}                                    \
	} while(0)

#define try(this)                                 \
{                                                 \
	ExceptionManager_Check(this);                 \
	ExceptionManager *__exc_mgr = this;           \
	ExceptionManager_Push(__exc_mgr,              \
		StackNew(ExceptionManager_Record)         \
	);                                            \
	bool __exc_rethrow = false;                   \
	if (setjmp(__exc_mgr->cur->jmpBuffer) == 0) {

#define catch(c, _e)                         \
	} else if (__exc_mgr->e.p == c) {        \
		UNUSED Exception *_e = &__exc_mgr->e;

#define catchAny(_e)                         \
	} else if (true) {                       \
		UNUSED Exception *_e = &__exc_mgr->e;

#define finally               \
	} else {                  \
		__exc_rethrow = true; \
	}

#define tryEnd                             \
	ExceptionManager_Pop(__exc_mgr);       \
	if (__exc_rethrow) {                   \
		ExceptionManager_Raise(__exc_mgr); \
	}                                      \
} do { } while(0)

#define excBreak                     \
	ExceptionManager_Pop(__exc_mgr); \
	break

#define excContinue                  \
	ExceptionManager_Pop(__exc_mgr); \
	continue

#define excReturn                    \
	ExceptionManager_Pop(__exc_mgr); \
	return

#define excGoto                      \
	ExceptionManager_Pop(__exc_mgr); \
	goto

#define excRethrow \
	__exc_rethrow = true

#define excThrow(...)                \
	ExceptionManager_Pop(__exc_mgr); \
	throw(__VA_ARGS__)

#define throw(...) \
	ExceptionManager_Throw(__VA_ARGS__)
