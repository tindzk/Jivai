#import <setjmp.h>

#import "Types.h"
#import "Runtime.h"
#import "Compiler.h"
#import "Exception.h"

typedef struct {
	struct _ExceptionManager_Record *cur;
	Exception e;
} ExceptionManager;

typedef struct _ExceptionManager_Record {
	struct _ExceptionManager_Record *prev; /* Linked list. */
	jmp_buf jmpBuffer;
} ExceptionManager_Record;

void ExceptionManager_Init(ExceptionManager *this);
void ExceptionManager_Raise(ExceptionManager *this);
void ExceptionManager_Push(ExceptionManager *this,
	 ExceptionManager_Record *record);
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

#define throw(this, c)  \
	do {                                 \
		ExceptionManager_Check(this);    \
		Exception_SetTrace((this)->e);   \
		Exception_SetModule((this)->e);  \
		Exception_SetOrigin((this)->e);  \
		Exception_SetCode((this)->e, c); \
		ExceptionManager_Raise(this);    \
	} while(0)

#define try(this)                                 \
{                                                 \
	ExceptionManager_Check(this);                 \
	ExceptionManager *__exc_mgr = this;           \
	ExceptionManager_Push(__exc_mgr,              \
		StackNew(ExceptionManager_Record));       \
	bool __exc_rethrow = false;                   \
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

#define finally     \
	} else {        \
		excRethrow; \
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
