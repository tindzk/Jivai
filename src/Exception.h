#import <setjmp.h>

#import "Types.h"
#import "Macros.h"
#import "String.h"
#import "Runtime.h"

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

typedef struct _ExceptionManager_Record {
	struct _ExceptionManager_Record *prev; /* Linked list. */
	jmp_buf jmpBuffer;
} ExceptionManager_Record;

typedef struct {
	ExceptionManager_Record *cur;
	Exception e;
} ExceptionManager;

void ExceptionManager_Init(ExceptionManager *this);
void ExceptionManager_Raise(ExceptionManager *this, size_t code);
void ExceptionManager_Push(ExceptionManager *this,
	 ExceptionManager_Record *_record);
void ExceptionManager_Pop(ExceptionManager *this);
Exception* ExceptionManager_GetMeta(ExceptionManager *this);
void ExceptionManager_Print(ExceptionManager *this, size_t code);

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

#if Exception_Safety
#define ExceptionManager_Check(this)                        \
	do {                                                    \
		if ((this) == NULL) {                               \
			String_Print(String(                            \
				"The exception manager in "));              \
			String_Print(String(__FILE__));                 \
			String_Print(String(" is not initialized!\n")); \
			Runtime_Exit(ExitStatus_Failure);               \
		}                                                   \
	} while(0)
#else
#define ExceptionManager_Check(...) \
	do { } while (0)
#endif

#define ExceptionManager_SetException(this, c) \
	Exception_SetTrace((this)->e);             \
	Exception_SetOrigin((this)->e);            \
	Exception_SetCode((this)->e, c);

#define throw(this, e)                               \
	do {                                             \
		ExceptionManager_Check(this);                \
		ExceptionManager_SetException(this, e);      \
		ExceptionManager_Raise(this, CurModule + e); \
	} while(0)

#define try(this)                                 \
{                                                 \
	ExceptionManager_Check(this);                 \
	ExceptionManager *__exc_mgr = this;           \
	ExceptionManager_Push(__exc_mgr,              \
		StackNew(ExceptionManager_Record));       \
	bool __exc_rethrow = false;                   \
	bool __exc_ignore_finally = false;            \
	__label__ __exc_finally;                      \
	void *__exc_return_ptr = && __exc_done;       \
	size_t e = setjmp(__exc_mgr->cur->jmpBuffer); \
	if (e == 0) {                                 \

#define clean                            \
		ExceptionManager_Pop(__exc_mgr); \
	} else if (ExceptionManager_Pop(__exc_mgr), false) { }


#define catch(module, code) \
	else if (e == Modules_##module + code)

#define catchModule(module) \
	else if (e >= Modules_##module && e < Modules_##module + Manifest_GapSize)

#define catchAny \
	else if (true)

#define finally                \
	else {                     \
		__exc_rethrow = true;  \
	}                          \
	__exc_finally:             \
	if (!__exc_ignore_finally)

#define tryEnd                                \
	if (__exc_rethrow) {                      \
		ExceptionManager_Raise(__exc_mgr, e); \
	}                                         \
	goto *__exc_return_ptr;                   \
	__exc_done: if(1) { }                     \
} do { } while(0)

/* Use the current line number for a unique label. */
#define __exc_label \
	simpleConcat(__exc_return_label, __LINE__)

/* Goes to finally block and then returns. */
#define __exc_goto_finally             \
	__exc_return_ptr = && __exc_label; \
	goto __exc_finally;                \
	__exc_label:                       \
	__exc_return_ptr = && __exc_done;  \
	__exc_ignore_finally = true;

#define excBreak       \
	__exc_goto_finally \
	break

#define excContinue    \
	__exc_goto_finally \
	continue

#define excReturn      \
	__exc_goto_finally \
	return

#define excGoto        \
	__exc_goto_finally \
	goto

#define excThrow(this, code)                   \
	e = CurModule + code;                      \
	ExceptionManager_SetException(this, code); \
	__exc_rethrow = true;                      \
	goto __exc_finally
