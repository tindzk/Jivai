#import "Exception.h"
#import "App.h"

#undef self
#define self Exception

ExceptionManager __exc_mgr;

inline sdef(void, Raise, size_t code) {
	if (__exc_mgr.cur == NULL) {
		scall(Print, code);
		Runtime_Exit(ExitStatus_Failure);
	}

	longjmp(__exc_mgr.cur->jmpBuffer, code);
}

inline sdef(void, Push, ref(Buffer) *buf) {
	buf->prev = __exc_mgr.cur;
	__exc_mgr.cur = buf;
}

inline sdef(void, Pop) {
	if (__exc_mgr.cur != NULL) {
		__exc_mgr.cur = __exc_mgr.cur->prev;
	}
}

inline sdef(ref(Record) *, GetMeta) {
	return &__exc_mgr.e;
}

sdef(void, Print, size_t code) {
#if Exception_SaveOrigin
	String fmt = String_Format(
		String("Uncaught exception %.% (in %)\n"),
		String_FromNul(Manifest_ResolveName(code)),
		__exc_mgr.e.scode,
		__exc_mgr.e.func);
#else
	String fmt = String_Format(
		String("Uncaught exception %.%\n"),
		String_FromNul(Manifest_ResolveName(code)),
		__exc_mgr.e.scode);
#endif

	String_Print(fmt, true);

	String_Destroy(&fmt);
}

Constructor {
	__exc_mgr.cur = NULL;
}
