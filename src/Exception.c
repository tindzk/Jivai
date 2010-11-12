#import "Exception.h"
#import "App.h"

#undef self
#define self ExceptionManager

inline def(void, Init) {
	this->cur = NULL;
}

inline def(void, Raise, size_t code) {
	if (this->cur == NULL) {
		ExceptionManager_Print(this, code);
		Runtime_Exit(ExitStatus_Failure);
	}

	longjmp(this->cur->jmpBuffer, code);
}

inline def(void, Push, ref(Record) *_record) {
	_record->prev = this->cur;
	this->cur = _record;
}

inline def(void, Pop) {
	if (this->cur != NULL) {
		this->cur = this->cur->prev;
	}
}

inline def(Exception *, GetMeta) {
	return &this->e;
}

def(void, Print, size_t code) {
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
