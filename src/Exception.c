#import "String.h"
#import "Exception.h"

void Exception_Print(Exception *e) {
#if Exception_SaveOrigin
	String_FmtPrint(
		String("Uncaught exception %.% (in %)\n"),
		Module_ResolveName(e->module),
		e->scode,
		e->func);
#else
	String_FmtFormat(
		String("Uncaught exception %.% '%'\n"),
		Module_ResolveName(e->module),
		e->scode);
#endif
}
