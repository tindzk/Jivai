#import "TestSuite.h"

#define self tsException

// @exc CustomException

class {
	size_t cntTry;
	size_t cntEnd;
	size_t cntFinally;

	void *old;
};

tsRegister("Exception") {
	return true;
}

extern ExceptionManager __exc_mgr;

tsInit {
	this->cntTry     = 0;
	this->cntEnd     = 0;
	this->cntFinally = 0;

	this->old = __exc_mgr.cur;
}

def(void, Reset) {
	this->cntTry     = 0;
	this->cntEnd     = 0;
	this->cntFinally = 0;
}

def(void, ThrowExc) {
	throw(CustomException);
}

def(void, Start) {
	call(Reset);

	try {
		this->cntTry++;
		call(ThrowExc);
		this->cntTry++; /* Must be ignored. */
	} finally {
		this->cntFinally++;
	} tryEnd;

	this->cntEnd++;
}

tsCase(Acute, "Finally block with an exception thrown") {
	/* Just ignore all exceptions. Otherwise the application would
	 * crash without verifying the test case.
	 */
	try {
		call(Start);
		this->cntTry++; /* Must be ignored. */
	} catchAny {
	} finally {
	} tryEnd;

	/* The stuff after the `tryEnd' shouldn't be executed. */
	Assert($("'try' executed"),     this->cntTry     == 1);
	Assert($("'end' not executed"), this->cntEnd     == 0);
	Assert($("'finally' executed"), this->cntFinally == 1);
}

def(void, StartSecondTest) {
	call(Reset);

	try {
		this->cntTry++;
	} finally {
		this->cntFinally++;
	} tryEnd;

	this->cntEnd++;
}

tsCase(Acute, "Finally block without an exception thrown") {
	try {
		call(StartSecondTest);
	} catchAny {
	} finally {
	} tryEnd;

	/* No exception thrown. Execute everything. */
	Assert($("'try' executed"),     this->cntTry     == 1);
	Assert($("'end' executed"),     this->cntEnd     == 1);
	Assert($("'finally' executed"), this->cntFinally == 1);
}

tsCase(Acute, "Empty exception block") {
	/* This test case doesn't do any asserts. It merely exists with
	 * the purpose of causing compiler or runtime warnings/errors.
	 * To verify its functionality, this test case shouldn't result
	 * in any errors when run in Valgrind.
	 */

	try {
	} finally {
	} tryEnd;
}

tsCase(Acute, "Safety") {
	Assert($("No stack corruption"),
		__exc_mgr.cur == this->old);
}
