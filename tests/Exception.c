#import "TestSuite.h"
#import "App.h"

enum {
	excCustomException = excOffset
};

#undef self
#define self tsException

class(self) {
	size_t cntTry;
	size_t cntEnd;
	size_t cntFinally;
};

tsRegister("Exception handling") {
	return true;
}

tsInit {
	this->cntTry     = 0;
	this->cntEnd     = 0;
	this->cntFinally = 0;
}

def(void, Reset) {
	this->cntTry     = 0;
	this->cntEnd     = 0;
	this->cntFinally = 0;
}

def(void, ThrowExc) {
	throw(excCustomException);
}

def(void, Start) {
	call(Reset);

	try {
		this->cntTry++;
		call(ThrowExc);
		this->cntTry++; /* Must be ignored. */
	} clean finally {
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
	} clean catchAny {
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
	} clean finally {
		this->cntFinally++;
	} tryEnd;

	this->cntEnd++;
}

tsCase(Acute, "Finally block without an exception thrown") {
	try {
		call(StartSecondTest);
	} clean catchAny {
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
	} clean finally {
	} tryEnd;
}

tsFinalize;
