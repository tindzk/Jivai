#import "Exception.h"
#import "Terminal.h"

#define self Exception

ExceptionManager __exc_mgr = {
	.cur = NULL
};

sdef(void, Print, int code) {
#if Exception_SaveOrigin
	FmtString fmt = FmtString(
		$("Uncaught exception %.% (in %)\n"),
		String_FromNul(Manifest_ResolveName(code)),
		__exc_mgr.details.scode,
		Exception_GetOrigin());
#else
	FmtString fmt = FmtString(
		$("Uncaught exception %.%\n"),
		String_FromNul(Manifest_ResolveName(code)),
		__exc_mgr.details.scode);
#endif

	Terminal term = Terminal_New(File_StdIn, File_StdErr, false);
	Terminal_PrintFmt(&term, fmt);
	Terminal_Destroy(&term);
}

sdef(void, Shutdown, int code) {
	scall(Print, code);
	Runtime_Exit(ExitStatus_Failure);
}
