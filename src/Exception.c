#import "Exception.h"

#define self Exception

ExceptionManager __exc_mgr = {
	.cur = NULL
};

sdef(String, FormatAssert) {
#if Exception_SaveOrigin
	return String_Format(
		$("Assertion '%' failed (in %)."),
		Exception_GetMessage().rd,
		Exception_GetOrigin());
#else
	return String_Format(
		$("Assertion '%' failed."),
		Exception_GetMessage());
#endif
}

sdef(String, Format, int code) {
	if (code == ref(AssertFailed)) {
		return scall(FormatAssert);
	}

#if Exception_SaveOrigin
	return String_Format(
		$("Uncaught exception %.% (in %)"),
		String_FromNul(Manifest_ResolveName(code)),
		String_FromNul(Manifest_ResolveCode(code)),
		Exception_GetOrigin());
#else
	return String_Format(
		$("Uncaught exception %.%"),
		String_FromNul(Manifest_ResolveName(code)),
		String_FromNul(Manifest_ResolveCode(code)));
#endif
}

sdef(void, Print, int code) {
	String msg = scall(Format, code);
	System_Err(msg.rd);
	String_Destroy(&msg);

	System_Err($("\n"));
}

sdef(void, Shutdown, int code) {
	scall(Print, code);
	System_Exit(ExitStatus_Failure);
}
