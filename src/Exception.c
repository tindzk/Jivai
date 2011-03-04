#import "Exception.h"
#import "Terminal.h"

#define self Exception

ExceptionManager __exc_mgr = {
	.cur = NULL
};

inline sdef(void, Raise, int code) {
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

sdef(void, Print, int code) {
#if Exception_SaveOrigin
	FmtString fmt = FmtString(
		$("Uncaught exception %.% (in %)\n"),
		String_FromNul(Manifest_ResolveName(code)),
		__exc_mgr.e.scode,
		__exc_mgr.e.func);
#else
	FmtString fmt = FmtString(
		$("Uncaught exception %.%\n"),
		String_FromNul(Manifest_ResolveName(code)),
		__exc_mgr.e.scode);
#endif

	Terminal term = Terminal_New(File_StdIn, File_StdErr, false);
	Terminal_PrintFmt(&term, fmt);
	Terminal_Destroy(&term);
}
