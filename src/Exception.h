#import <setjmp.h>

#import "Types.h"
#import "Macros.h"
#import "String.h"
#import "System.h"
#import "Backtrace.h"

#define self Exception

// @exc AssertFailed

#ifndef Exception_SaveOrigin
#define Exception_SaveOrigin 1
#endif

#ifndef Exception_SaveTrace
#define Exception_SaveTrace 0
#endif

#ifndef Exception_TraceSize
#define Exception_TraceSize 15
#endif

#ifndef Exception_Assert
#define Exception_Assert 1
#endif

/* Stack frame. */
record(ref(Frame)) {
	struct ref(Frame) *prev; /* Linked list. */
	jmp_buf jmpBuffer;
};

record(ExceptionManager) {
	ref(Frame) *cur;

	/* This structure contains more detailed information about the most
	 * recently thrown exception. The exception itself is only supplied
	 * in a numerical code via longjmp(). See Exception_Raise().
	 */
	struct {
#if Exception_SaveOrigin
		OmniString func;
#endif

#if Exception_SaveTrace
		struct {
			void *buf[Exception_TraceSize];
			size_t len;
		} trace;
#endif

		OmniString msg;
		void *data;
	} details;
};

extern ExceptionManager __exc_mgr;

sdef(String, Format, int code);
sdef(void, Print, int code);
sdef(void, Shutdown, int code);

static inline sdef(void, Push, ref(Frame) *buf) {
	buf->prev = __exc_mgr.cur;
	__exc_mgr.cur = buf;
}

static inline sdef(void, Pop, int code) {
	if (__exc_mgr.cur == NULL) {
		scall(Shutdown, code);
	}

	__exc_mgr.cur = __exc_mgr.cur->prev;
}

static inline sdef(void, Raise, int code) {
	ref(Frame) *cur = __exc_mgr.cur;
	scall(Pop, code);
	longjmp(cur->jmpBuffer, code);
}

static inline sdef(void, SetMessage, OmniString msg) {
	__exc_mgr.details.msg = msg;
}

static inline sdef(OmniString, GetMessage) {
	return __exc_mgr.details.msg;
}

static inline sdef(void, SetData, void *data) {
	__exc_mgr.details.data = data;
}

static inline sdef(void *, GetData) {
	return __exc_mgr.details.data;
}

#if Exception_SaveOrigin
	static inline void Exception_SetOrigin(OmniString func) {
		__exc_mgr.details.func = func;
	}

	static inline RdString Exception_GetOrigin(void) {
		return __exc_mgr.details.func.rd;
	}
#else
	static inline void Exception_SetOrigin(__unused OmniString func) { }
	static inline RdString Exception_GetOrigin(void) {
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

#define Exception_SetException()      \
	Exception_SetTrace();             \
	Exception_SetOrigin($$(__func__)) \

#define throw(e)                  \
	do {                          \
		Exception_SetException(); \
		Exception_Raise(ref(e));  \
	} while(0)

#define try                                    \
{                                              \
	__label__ __exc_done;                      \
	__label__ __exc_finally;                   \
	/* Silence warnings about unused label. */ \
	if (0) goto __exc_finally;                 \
	bool __exc_rethrow = false;                \
	bool __exc_ignore_finally = false;         \
	void *__exc_return_ptr = && __exc_done;    \
	                                           \
	Exception_Frame __exc_frame;               \
	Exception_Push(&__exc_frame);              \
	                                           \
	int e = setjmp(__exc_frame.jmpBuffer);     \
	if (e == 0)

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
	if (!__exc_ignore_finally) { \
		if (e == 0) {            \
			Exception_Pop(-1);   \
		}                        \

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

/* Goes to finally block and returns to __exc_label. */
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
	Exception_SetException();        \
	__exc_rethrow = true;            \
	goto __exc_finally

#if Exception_Assert
	#define assert(expr)                             \
		if (!(expr)) {                               \
			Exception_SetMessage($$(#expr));         \
			Exception_SetException();                \
			Exception_Raise(Exception_AssertFailed); \
		}
#else
	#define assert(...)
#endif

#undef self
