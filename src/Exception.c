#import "Exception.h"

#define self Exception

ExceptionManager __exc_mgr = {
	.cur = NULL
};

sdef(OmniString, InternalFormat, RdString fmt, ...) {
	VarArg argptr;
	VarArg_Start(argptr, fmt);

	OmniString str = {
		.buf = __exc_mgr.buf
	};

	fwd(i, fmt.len) {
		if (fmt.buf[i] == '%') {
			RdString value = VarArg_Get(argptr, RdString);

			if (str.len + value.len > nElems(__exc_mgr.buf)) {
				goto error;
			}

			Memory_Copy(str.buf + str.len, value.buf, value.len);
			str.len += value.len;
		} else {
			if (str.len + 1 > nElems(__exc_mgr.buf)) {
				goto error;
			}

			str.buf[str.len] = fmt.buf[i];
			str.len++;
		}
	}

	VarArg_End(argptr);

	return str;

error:
	return $$("Error: Internal buffer too small!");
}

sdef(OmniString, FormatAssert) {
#if Exception_SaveOrigin
	return scall(InternalFormat,
		$("Assertion '%' failed (in %)."),
			Exception_GetMessage().rd,
			Exception_GetOrigin());
#else
	return String_Format(
		$("Assertion '%' failed."),
			Exception_GetMessage());
#endif
}

sdef(OmniString, Format, int code) {
	if (code == ref(AssertFailed)) {
		return scall(FormatAssert);
	}

#if Exception_SaveOrigin
	return scall(InternalFormat,
		$("Uncaught exception %.% (in %)"),
			String_FromNul(Manifest_ResolveName(code)),
			String_FromNul(Manifest_ResolveCode(code)),
			Exception_GetOrigin());
#else
	return scall(InternalFormat,
		$("Uncaught exception %.%"),
			String_FromNul(Manifest_ResolveName(code)),
			String_FromNul(Manifest_ResolveCode(code)));
#endif
}

sdef(void, Print, int code) {
	System_Err(scall(Format, code).rd);
	System_Err($("\n"));
}

sdef(void, Shutdown, int code) {
	scall(Print, code);
	System_Exit(ExitStatus_Failure);
}
