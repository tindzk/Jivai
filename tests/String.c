#import <String.h>

#import "TestSuite.h"

#define self tsString

class {
	String s;
};

tsRegister("String") {
	return true;
}

tsCase(Acute, "Allocation: String_New(0)") {
	this->s = String_New(0);

	Assert($("Empty buffer"), this->s.buf == NULL);
	Assert($("Length is 0"),  this->s.len == 0);
}

tsCase(Acute, "Allocation: String_New(16)") {
	this->s = String_New(16);

	Assert($("Non-empty buffer"),
		this->s.buf != NULL);

	Assert($("Size is 16"),
		String_GetSize(&this->s) == 16);

	Assert($("Length = 0"),
		this->s.len == 0);
}

tsCase(Acute, "Destruction") {
	String_Destroy(&this->s);

	/* String_Destroy() resets the structure. Now it's similar to
	 * String_New(0)'s except that it's not mutable. The reason for
	 * this is to detect double frees because String_Destroy()
	 * throws an exception when a string isn't mutable.
	 */

	Assert($("Empty buffer"),
		this->s.buf == (void *) 0xdeadbeef);

	Assert($("Length is 0"),
		this->s.len == 0);
}

tsCase(Acute, "Appending (2)") {
	String s = String_New(0);
	String append = $("Test.");

	size_t caught = false;

	try {
		String_Append(&s, append);
	} clean catch(String, DoubleFree) {
		caught = true;
	} finally {
	} tryEnd;

	Assert($("Heap strings are mutable"), !caught);

	/* Perform optimizations when the destination string is empty. */
	Assert($("Points to original string"),
		s.buf == append.buf &&
		s.len == append.len);

	String_Destroy(&s);
}

tsCase(Acute, "Appending (3)") {
	String s = String_New(1);
	String_Append(&s, $("Hello World."));

	Assert($("Not empty"), s.len > 0);
	Assert($("Equals"), String_Equals(s, $("Hello World.")));

	String_Destroy(&s);
}

tsCase(Acute, "Appending (4)") {
	String s = String_New(16);
	String_Append(&s, $("Hello World."));

	Assert($("Not empty"), s.len > 0);
	Assert($("Equals"), String_Equals(s, $("Hello World.")));

	String_Destroy(&s);
}

tsCase(Acute, "Resizing") {
	String s = String_New(1);
	String_Append(&s, 'a');

	/* Free the buffer. It differs from Destroy() by setting the buffer to NULL
	 * as opposed to 0xdeadbeef.
	 */
	String_Resize(&s, 0);

	Assert($("Length is 0"),    s.len == 0);
	Assert($("Buffer is NULL"), s.buf == NULL);

	/* Although the buffer was already freed by Resize(), this is valid. */
	String_Destroy(&s);

	Assert($("Empty buffer"),
		s.buf == (void *) 0xdeadbeef);
}

tsCase(Acute, "Cloning") {
	String s = $("Hello World.");

	String cloned = String_Clone(s);

	String_Destroy(&cloned);

	Assert($("Original string still accessible"),
		String_Equals(s, $("Hello World.")));
}

tsCase(Acute, "Copying") {
	String s = String_New(0);

	String copy = $("Hello World.");

	/* Multiple calls to check whether there are any leaks. */
	String_Copy(&s, copy);
	String_Copy(&s, copy);
	String_Copy(&s, copy);

	Assert($("Points to copy"),
		s.buf == copy.buf);
	Assert($("Has correct length"),
		s.len == copy.len);

	String_Destroy(&s);
}

tsCase(Acute, "Copying") {
	String s = String_New(1);

	String copy = $("Hello World.");

	String_Copy(&s, copy);
	String_Copy(&s, copy);
	String_Copy(&s, copy);

	Assert($("Does not point to copy"),
		s.buf != copy.buf);
	Assert($("Is resized"),
		String_GetSize(&s) >= copy.len);
	Assert($("Has correct length"),
		s.len == copy.len);

	String_Destroy(&s);
}

tsCase(Acute, "Copying") {
	String s = String_New(1);

	String copy = String_New(1);
	String_Append(&copy, $("Hello World."));

	String_Copy(&s, copy);
	String_Copy(&s, copy);
	String_Copy(&s, copy);

	Assert($("Does not point to copy"),
		s.buf != copy.buf);
	Assert($("Is resized"),
		String_GetSize(&s) >= copy.len);
	Assert($("Has correct length"),
		s.len == copy.len);

	String_Destroy(&copy);
	String_Destroy(&s);
}

tsCase(Acute, "Formatted appending") {
	String s = String_New(0);

	bool caught = false;

	try {
		String_Append(&s, FmtString($("Hello %.")));
	} clean catch(String, ElementMismatch) {
		caught = true;
	} finally {
	} tryEnd;

	Assert($("Detected mismatch"), caught);

	String_Destroy(&s);
}

tsCase(Acute, "Appending") {
	String s = $("Hello ");
	String_Append(&s, $("World."));
	Assert($("Equals"), String_Equals(s, $("Hello World.")));
	String_Destroy(&s);
}

tsCase(Acute, "Destruction of rodata strings") {
	String s = $("Hello World.");
	String_Destroy(&s);
}

tsCase(Acute, "Destruction of empty strings") {
	String s = String_New(0);
	String_Destroy(&s);
}

tsCase(Acute, "Double frees") {
	String s = String_New(0);
	String_Destroy(&s);

	bool caught = false;

	try {
		String_Destroy(&s);
	} clean catch(String, DoubleFree) {
		caught = true;
	} finally {

	} tryEnd;

	Assert($("Caught"), caught);
}

tsCase(Acute, "Slicing") {
	String s = $("Hello World.");

	String sliced = String_Slice(s, 2, 4);
	String_Destroy(&sliced);

	String_Append(&s, $("Hi."));

	Assert($("Equals"), String_Equals(s, $("Hello World.Hi.")));

	String_Destroy(&s);
}

/* Append to sliced. */
tsCase(Acute, "Slicing") {
	String s = $("Hello World.");

	String sliced = String_Slice(s, 2, 4);
	String_Append(&sliced, $("Hi."));
	Assert($("Equals"), String_Equals(sliced, $("llo Hi.")));
	String_Destroy(&sliced);

	String_Destroy(&s);
}

tsCase(Acute, "Split") {
	String s = String_New(0);
	String_Append(&s, $("Hello World."));
	String_Append(&s, $("Hello World."));

	StringArray *parts = String_Split(s, ' ');

	String_Destroy(&s);

	StringArray_Destroy(parts); /* Optional. */
	StringArray_Free(parts);
}

tsCase(Acute, "Split") {
	String s = String_New(0);
	String_Append(&s, $("Hello World."));
	String_Append(&s, $("Hello World."));

	String part = $("");
	Assert($("Return value"), String_Split(s, ' ', &part));
	Assert($("off=0 len=5"), part.buf == s.buf && part.len == 5);

	Assert($("Return value"), String_Split(s, ' ', &part));
	Assert($("off=6 len=11"), part.buf == s.buf + 6 && part.len == 11);

	Assert($("Return value"), String_Split(s, ' ', &part));
	Assert($("off=18 len=6"), part.buf == s.buf + 18 && part.len == 6);

	String_Destroy(&s);
}

tsCase(Acute, "Split with empty string") {
	String part = $("");
	Assert($("Return value"), String_Split($(""), ' ', &part) == false);
}

tsCase(Acute, "Joining") {
	String orig  = $("");
	String orig2 = $(" ");
	String orig3 = $("abc ab a");
	String orig4 = $("abc ab a ");

	StringArray *items  = String_Split(orig,  ' ');
	StringArray *items2 = String_Split(orig2, ' ');
	StringArray *items3 = String_Split(orig3, ' ');
	StringArray *items4 = String_Split(orig4, ' ');

	String joined  = StringArray_Join(items,  $(" "));
	String joined2 = StringArray_Join(items2, $(" "));
	String joined3 = StringArray_Join(items3, $(" "));
	String joined4 = StringArray_Join(items4, $(" "));

	Assert($("Empty"),         String_Equals(orig,  joined));
	Assert($("Non-empty"),     String_Equals(orig2, joined2));
	Assert($("Non-empty (2)"), String_Equals(orig3, joined3));
	Assert($("Non-empty (3)"), String_Equals(orig4, joined4));

	StringArray_Free(items4);
	StringArray_Free(items3);
	StringArray_Free(items2);
	StringArray_Free(items);

	String_Destroy(&joined4);
	String_Destroy(&joined3);
	String_Destroy(&joined2);
	String_Destroy(&joined);
}

tsFinalize;
