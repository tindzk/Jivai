#include "ExceptionManager.h"

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
#ifdef Exception_SaveOrigin
	String s = String_Format(
		String("Uncaught exception '%' in %:%\n"),
		*(String *) e->p,
		e->file,
		Integer_ToString(e->line));
#else
	String s = String_Format(
		String("Uncaught exception '%'\n"),
		*(String *) e->p);
#endif

	String_Print(s);

	String_Destroy(&s);
}
