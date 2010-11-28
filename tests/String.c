#import <String.h>

#import "TestSuite.h"

#undef self
#define self tsString

class {
	String s;
};

tsRegister("String") {
	return true;
}

tsCase(Acute, "Allocation: HeapString(0)") {
	this->s = HeapString(0);

	Assert($("Is mutable"),
		this->s.mutable == true);

	Assert($("Empty buffer"),
		this->s.buf == NULL);

	Assert($("Size and length = 0"),
		this->s.len  == 0
	 && this->s.size == 0);
}

tsCase(Acute, "Allocation: HeapString(16)") {
	this->s = HeapString(16);

	Assert($("Is mutable"),
		this->s.mutable == true);

	Assert($("Non-empty buffer"),
		this->s.buf != NULL);

	Assert($("Size = 16"),
		this->s.size == 16);

	Assert($("Length = 0"),
		this->s.len == 0);
}

tsCase(Acute, "Destruction") {
	String_Destroy(&this->s);

	/* String_Destroy() resets the structure. Now it's similar to
	 * HeapString(0)'s except that it's not mutable. The reason for
	 * this is to detect double frees because String_Destroy()
	 * throws an exception when a string isn't mutable.
	 */

	Assert($("Must be volatile"),
		this->s.mutable == false);

	Assert($("Empty buffer"),
		this->s.buf == NULL);

	Assert($("Size and length = 0"),
		this->s.len  == 0
	 && this->s.size == 0);
}

tsFinalize;
