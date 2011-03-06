#import <setjmp.h>

#import "Types.h"
#import "Macros.h"
#import "String.h"
#import "Runtime.h"
#import "Backtrace.h"

#define self Exception

#ifndef Exception_SaveOrigin
#define Exception_SaveOrigin 1
#endif

#ifndef Exception_SaveTrace
#define Exception_SaveTrace 0
#endif

#ifndef Exception_TraceSize
#define Exception_TraceSize 15
#endif

record(ref(Buffer)) {
	struct ref(Buffer) *prev; /* Linked list. */
	jmp_buf jmpBuffer;
};

record(ExceptionManager) {
	ref(Buffer) *cur;

	/* This structure contains more detailed information about the most
	 * recently thrown exception. The exception itself is only supplied
	 * in a numerical code via longjmp(). See Exception_Raise().
	 */
	struct {
#if Exception_SaveOrigin
		ProtString func;
#endif

#if Exception_SaveTrace
		struct {
			void *buf[Exception_TraceSize];
			size_t len;
		} trace;
#endif

		ProtString scode;
		ProtString msg;
		void *data;
	} details;
};

extern ExceptionManager __exc_mgr;

sdef(String, Format, int code);
sdef(void, Print, int code);
sdef(void, Shutdown, int code);

static inline sdef(void, Raise, int code) {
	if (__exc_mgr.cur == NULL) {
		scall(Shutdown, code);
	}

	longjmp(__exc_mgr.cur->jmpBuffer, code);
}

static inline sdef(void, Push, ref(Buffer) *buf) {
	buf->prev = __exc_mgr.cur;
	__exc_mgr.cur = buf;
}

static inline sdef(void, Pop) {
	if (__exc_mgr.cur != NULL) {
		__exc_mgr.cur = __exc_mgr.cur->prev;
	}
}

static inline sdef(void, SetMessage, ProtString msg) {
	__exc_mgr.details.msg = msg;
}

static inline sdef(ProtString, GetMessage) {
	return __exc_mgr.details.msg;
}

static inline sdef(void, SetData, void *data) {
	__exc_mgr.details.data = data;
}

static inline sdef(void *, GetData) {
	return __exc_mgr.details.data;
}

#if Exception_SaveOrigin
	static inline void Exception_SetOrigin(ProtString func) {
		__exc_mgr.details.func = func;
	}

	static inline ProtString Exception_GetOrigin(void) {
		return __exc_mgr.details.func;
	}
#else
	static inline void Exception_SetOrigin(__unused ProtString func) { }
	static inline ProtString Exception_GetOrigin(void) {
		return $("");
	}
#endif

#if Exception_SaveTrace
	#define Exception_SetTrace()             \
		__exc_mgr.details.trace.len =        \
			Backtrace_GetTrace(              \
				__exc_mgr.details.trace.buf, \
				Exception_TraceSize)

	static inline sdef(void **, GetTraceBuffer) {
		return __exc_mgr.details.trace.buf;
	}

	static inline sdef(size_t, GetTraceLength) {
		return __exc_mgr.details.trace.len;
	}
#else
	#define Exception_SetTrace(e)
#endif

#define Exception_SetException(c)     \
	Exception_SetTrace();             \
	Exception_SetOrigin($(__func__)); \
	__exc_mgr.details.scode = $(#c)

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
