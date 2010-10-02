#import "String.h"
#import "Exception.h"

void Exception_Print(Exception *e) {
#if Exception_SaveOrigin
	String fmt = String_Format(
		String("Uncaught exception %.% (in %)\n"),
		Module_ResolveName(e->module),
		e->scode,
		e->func);
#else
	String fmt = String_Format(
		String("Uncaught exception %.% '%'\n"),
		Module_ResolveName(e->module),
		e->scode);
#endif

	String_Print(fmt, true);

	String_Destroy(&fmt);
}
