#import <String.h>

#import "TestSuite.h"

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

tsCase(Acute, "Appending") {
	String s = $("Hello World.");

	size_t caught = false;

	try {
		String_Append(&s, $("Test."));
	} clean catch(String, NotMutable) {
		caught = true;
	} finally {
	} tryEnd;

	Assert($("rodata strings are not mutable"), caught);
}

tsCase(Acute, "Appending (2)") {
	String s = HeapString(0);
	String append = $("Test.");

	size_t caught = false;

	try {
		String_Append(&s, append);
	} clean catch(String, NotMutable) {
		caught = true;
	} finally {
	} tryEnd;

	Assert($("Heap strings are mutable"), !caught);

	/* Perform optimizations when the destination string is empty. */
	Assert($("Points to original string"),
		s.buf == append.buf &&
		s.len == append.len);
	Assert($("`mutable' untouched"),
		s.mutable == true);
	Assert($("`size' is 0"),
		s.size == 0);

	String_Destroy(&s);
}

tsCase(Acute, "Appending (3)") {
	String s = HeapString(1);
	String_Append(&s, $("Hello World."));

	Assert($("`mutable' untouched"),
		s.mutable == true);
	Assert($("Is resized"),
		s.size > 1);
	Assert($("Not empty"),
		s.len > 0);

	String_Destroy(&s);
}

tsCase(Acute, "Cloning") {
	String s = $("Hello World.");

	String cloned = String_Clone(s);

	Assert($("Points to original string"),
		cloned.buf == s.buf &&
		cloned.len == s.len);
	Assert($("Is mutable"),
		cloned.mutable == true);
	Assert($("`size' is 0"),
		cloned.size == 0);
}

tsCase(Acute, "Copying") {
	String s = HeapString(0);

	String copy = $("Hello World.");

	/* Multiple calls to check whether there are any leaks. */
	String_Copy(&s, copy);
	String_Copy(&s, copy);
	String_Copy(&s, copy);

	Assert($("Points to copy"),
		s.buf == copy.buf &&
		s.len == copy.len);
	Assert($("Is mutable"),
		s.mutable == true);
	Assert($("`size' is 0"),
		s.size == 0);

	String_Destroy(&s);
}

tsCase(Acute, "Copying (2)") {
	String s = HeapString(1);

	String copy = $("Hello World.");

	String_Copy(&s, copy);
	String_Copy(&s, copy);
	String_Copy(&s, copy);

	Assert($("Does not point to copy"),
		s.buf != copy.buf);
	Assert($("Is resized"),
		s.size == copy.len);
	Assert($("Is mutable"),
		s.mutable == true);
	Assert($("Has correct length"),
		s.len == copy.len);

	String_Destroy(&s);
}

tsCase(Acute, "Copying (2)") {
	String s = HeapString(1);

	String copy = $("Hello World.");

	String_Copy(&s, copy);
	String_Copy(&s, copy);
	String_Copy(&s, copy);

	Assert($("Does not point to copy"),
		s.buf != copy.buf);
	Assert($("Is resized"),
		s.size == copy.len);
	Assert($("Is mutable"),
		s.mutable == true);
	Assert($("Has correct length"),
		s.len == copy.len);

	String_Destroy(&s);
}

tsCase(Acute, "Copying (3)") {
	String s = HeapString(1);

	String copy = HeapString(1);
	String_Append(&copy, $("Hello World."));

	String_Copy(&s, copy);
	String_Copy(&s, copy);
	String_Copy(&s, copy);

	Assert($("Does not point to copy"),
		s.buf != copy.buf);
	Assert($("Is resized"),
		s.size == copy.len);
	Assert($("Is mutable"),
		s.mutable == true);
	Assert($("Has correct length"),
		s.len == copy.len);

	String_Destroy(&copy);
	String_Destroy(&s);
}

tsFinalize;
