#import <setjmp.h>

#import "Types.h"
#import "Runtime.h"
#import "Compiler.h"
#import "Exception.h"

typedef struct _ExceptionManager_Record {
	struct _ExceptionManager_Record *prev; /* Linked list. */
	jmp_buf jmpBuffer;
} ExceptionManager_Record;

typedef struct {
	struct _ExceptionManager_Record *cur;
	Exception e;
} ExceptionManager;

void ExceptionManager_Init(ExceptionManager *this);
void ExceptionManager_Raise(ExceptionManager *this);
void ExceptionManager_Push(ExceptionManager *this,
	 ExceptionManager_Record *_record);
void ExceptionManager_Pop(ExceptionManager *this);

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
	Exception_SetModule((this)->e);            \
	Exception_SetOrigin((this)->e);            \
	Exception_SetCode((this)->e, c)            \

#define throw(this, e)                          \
	do {                                        \
		ExceptionManager_Check(this);           \
		ExceptionManager_SetException(this, e); \
		ExceptionManager_Raise(this);           \
	} while(0)

#define try(this)                                 \
{                                                 \
	ExceptionManager_Check(this);                 \
	ExceptionManager *__exc_mgr = this;           \
	ExceptionManager_Push(__exc_mgr,              \
		StackNew(ExceptionManager_Record));       \
	bool __exc_rethrow = false;                   \
	__label__ __exc_finally;                      \
	void *__exc_return_ptr = && __exc_done;       \
	if (setjmp(__exc_mgr->cur->jmpBuffer) == 0) {

#define catch(_module, _code, _e)               \
	} else if (__exc_mgr->e.module == _module   \
			&& __exc_mgr->e.code   == _code)    \
	{                                           \
		__unused Exception *_e = &__exc_mgr->e;

#define catchModule(_module, _e)                 \
	} else if (__exc_mgr->e.module == _module) { \
		__unused Exception *_e = &__exc_mgr->e;

#define catchAny(_e)                            \
	} else if (true) {                          \
		__unused Exception *_e = &__exc_mgr->e;

#define finally               \
	} else {                  \
		__exc_rethrow = true; \
	}                         \
	__exc_finally:

#define tryEnd                             \
	ExceptionManager_Pop(__exc_mgr);       \
	if (__exc_rethrow) {                   \
		ExceptionManager_Raise(__exc_mgr); \
	}                                      \
	goto *__exc_return_ptr;                \
	__exc_done: if(1) { }                  \
} do { } while(0)

/* Use the current line number for a unique label. */
#define __exc_label \
	simpleConcat(__exc_return_label, __LINE__)

/* Goes to finally block and then returns. */
#define __exc_goto_finally             \
	__exc_return_ptr = && __exc_label; \
	goto __exc_finally;                \
	__exc_label:                       \

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

/* Reuses current exception by overwriting its contents. */
#define excThrow(this, e)                   \
	ExceptionManager_SetException(this, e); \
	__exc_rethrow = true;                   \
	goto __exc_finally
