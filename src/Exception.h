#import <setjmp.h>

#import "Types.h"
#import "Macros.h"
#import "String.h"
#import "Runtime.h"

#define self Exception

#ifndef Exception_SaveOrigin
#define Exception_SaveOrigin 1
#endif

#ifndef Exception_SaveData
#define Exception_SaveData 0
#endif

#ifndef Exception_SaveMessage
#define Exception_SaveMessage 0
#endif

#ifndef Exception_SaveTrace
#define Exception_SaveTrace 0
#endif

#ifndef Exception_TraceSize
#define Exception_TraceSize 15
#endif

/* This structure contains more detailed information about the most
 * recently thrown exception. The exception itself is only supplied
 * in a numerical code via longjmp(). See Exception_Raise().
 */
record(ref(Record)) {
#if Exception_SaveOrigin
	String func;
#endif

#if Exception_SaveMessage
	String msg;
#endif

#if Exception_SaveData
	void *data;
#endif

	String scode;

#if Exception_SaveTrace
	void *trace[Exception_TraceSize];
	size_t traceItems;
#endif
};

record(ref(Buffer)) {
	struct ref(Buffer) *prev; /* Linked list. */
	jmp_buf jmpBuffer;
};

record(ExceptionManager) {
	ref(Buffer) *cur;
	ref(Record) e;
};

ExceptionManager __exc_mgr;

sdef(void, Init);
sdef(void, Raise, int code);
sdef(void, Push, ref(Buffer) *buf);
sdef(void, Pop);
sdef(ref(Record) *, GetMeta);
sdef(void, Print, int code);

#if Exception_SaveOrigin
#define Exception_SetOrigin() \
	__exc_mgr.e.func = String(__func__)
#else
#define Exception_SetOrigin() \
	do { } while(0)
#endif

#if Exception_SaveTrace
#undef self

#import "Backtrace.h"

#define Exception_SetTrace()     \
	__exc_mgr.e.traceItems =     \
		Backtrace_GetTrace(      \
			__exc_mgr.e.trace,   \
			Exception_TraceSize)
#else
#define Exception_SetTrace(e) \
	do { } while (0)

#define self Exception
#endif

#if Exception_SaveData
#define Exception_SetData(_data) \
	__exc_mgr.e.ptr = _data
#else
#define Exception_SetData(_data) \
	do { } while (0)
#endif

#if Exception_SaveMessage
#define Exception_SetMessage(_msg) \
	__exc_mgr.e.msg = _msg
#else
#define Exception_SetMessage(_msg) \
	do { } while (0)
#endif

#define Exception_SetCode(c) \
	__exc_mgr.e.scode = String(#c)

#define Exception_SetException(c) \
	Exception_SetTrace();         \
	Exception_SetOrigin();        \
	Exception_SetCode(c);

#define throw(e)                   \
	do {                           \
		Exception_SetException(e); \
		Exception_Raise(ref(e));   \
	} while(0)

#define try                                 \
{                                           \
	__label__ __exc_finally;                \
	bool __exc_rethrow = false;             \
	bool __exc_ignore_finally = false;      \
	void *__exc_return_ptr = && __exc_done; \
	                                        \
	Exception_Buffer __exc_buffer;          \
	Exception_Push(&__exc_buffer);          \
	                                        \
	int e = setjmp(__exc_buffer.jmpBuffer); \
	if (e == 0) {

#define clean                              \
		Exception_Pop();                   \
	} else if (Exception_Pop(), false) { }

#define catch(module, code) \
	else if (e == module##_##code)

#define catchModule(module)                 \
	else if (e > Modules_##module &&        \
			 e < Modules_##module##_Length)

#define catchAny \
	else if (true)

#define finally                  \
	else {                       \
		__exc_rethrow = true;    \
	}                            \
	__exc_finally:               \
	if (!__exc_ignore_finally) {

#define tryEnd                  \
		if (__exc_rethrow) {    \
			Exception_Raise(e); \
		}                       \
		goto *__exc_return_ptr; \
	}                           \
	__exc_done: if (1) { }      \
} do { } while(0)

/* Use the current line number for a unique label. */
#define __exc_label \
	simpleConcat(__exc_return_label, __LINE__)

/* Goes to finally block and then returns. */
#define __exc_goto_finally             \
	__exc_return_ptr = && __exc_label; \
	goto __exc_finally;                \
	__exc_label:                       \
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

#define excThrow(code)               \
	e = tripleConcat(self, _, code); \
	Exception_SetException(code);    \
	__exc_rethrow = true;            \
	goto __exc_finally

#undef self
