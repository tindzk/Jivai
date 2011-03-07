#import "Exception.h"

#define self Exception

ExceptionManager __exc_mgr = {
	.cur = NULL
};

sdef(String, Format, int code) {
#if Exception_SaveOrigin
	return String_Format(
		$("Uncaught exception %.% (in %)"),
		String_FromNul(Manifest_ResolveName(code)),
		__exc_mgr.details.scode,
		Exception_GetOrigin());
#else
	return String_Format(
		$("Uncaught exception %.%"),
		String_FromNul(Manifest_ResolveName(code)),
		__exc_mgr.details.scode);
#endif
}

sdef(void, Print, int code) {
	String msg = scall(Format, code);
	File_Write(File_StdErr, msg.prot);
	String_Destroy(&msg);

	File_Write(File_StdErr, $("\n"));
}

sdef(void, Shutdown, int code) {
	scall(Print, code);
	Runtime_Exit(ExitStatus_Failure);
}
