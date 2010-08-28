#import "String.h"
#import "ExceptionManager.h"

inline void ExceptionManager_Init(ExceptionManager *this) {
	this->cur = NULL;
}

inline void ExceptionManager_Raise(ExceptionManager *this) {
	if (this->cur == NULL) {
		Exception_Print(&this->e);
		exit(EXIT_FAILURE);
	}

	longjmp(this->cur->jmpBuffer, 1);
}

inline void ExceptionManager_Push(ExceptionManager *this,
								  ExceptionManager_Record *record)
{
	record->prev = this->cur;
	this->cur = record;
}

inline void ExceptionManager_Pop(ExceptionManager *this) {
	if (this->cur != NULL) {
		this->cur = this->cur->prev;
	}
}
