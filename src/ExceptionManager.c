#import "String.h"
#import "ExceptionManager.h"

inline void ExceptionManager_Init(ExceptionManager *this) {
	this->cur = NULL;
}

inline void ExceptionManager_Raise(ExceptionManager *this, size_t code) {
	if (this->cur == NULL) {
		ExceptionManager_Print(this, code);
		Runtime_Exit(ExitStatus_Failure);
	}

	longjmp(this->cur->jmpBuffer, code);
}

inline void ExceptionManager_Push(ExceptionManager *this,
								  ExceptionManager_Record *_record)
{
	_record->prev = this->cur;
	this->cur = _record;
}

inline void ExceptionManager_Pop(ExceptionManager *this) {
	if (this->cur != NULL) {
		this->cur = this->cur->prev;
	}
}

inline Exception* ExceptionManager_GetMeta(ExceptionManager *this) {
	return &this->e;
}

void ExceptionManager_Print(ExceptionManager *this, size_t code) {
#if Exception_SaveOrigin
	String fmt = String_Format(
		String("Uncaught exception %.% (in %)\n"),
		String_FromNul(Manifest_ResolveName(code)),
		this->e.scode,
		this->e.func);
#else
	String fmt = String_Format(
		String("Uncaught exception %.%\n"),
		String_FromNul(Manifest_ResolveName(code)),
		this->e.scode);
#endif

	String_Print(fmt, true);

	String_Destroy(&fmt);
}
