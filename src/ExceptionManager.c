#import "ExceptionManager.h"

void ExceptionManager_Init(ExceptionManager *this) {
	this->cur = NULL;
}

void ExceptionManager_Raise(ExceptionManager *this) {
	if (this->cur == NULL) {
		Exception_Print(&this->e);
		exit(EXIT_FAILURE);
	}

	longjmp(this->cur->jmpBuffer, 1);
}

void ExceptionManager_Push(ExceptionManager *this, ExceptionManager_Record *record) {
	record->prev = this->cur;
	this->cur = record;
}

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
