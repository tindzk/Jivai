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

tsCase(Acute, "StackString()") {
	String s = StackString(16);

	Assert($("Non-empty buffer"),
		s.buf != NULL);

	Assert($("Size is 16"),
		String_GetSize(&s) == 16);

	Assert($("Length = 0"),
		s.len == 0);

	String_Destroy(&s);
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

	Assert($("Not empty"),
		s.len > 0);

	String_Destroy(&s);
}

tsCase(Acute, "Appending (4)") {
	String s = StackString(16);
	String_Append(&s, $("Hello World."));

	Assert($("Not empty"),
		s.len > 0);

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

	/* Must be valid. Clones internally and sets `prev' to NULL. */
	String sliced = String_Slice(s, 2, 4);
	String_Append(&sliced, $("Hi."));
	Assert($("Equals"), String_Equals(sliced, $("llo Hi.")));
	String_Destroy(&sliced);

	String_Destroy(&s);
}

tsCase(Acute, "Slicing") {
	String s = $("Hello World.");

	String *sliced = String_SafeSlice(&s, 2, 4);
	Assert($("Equals"), String_Equals(*sliced, $("llo ")));

	String_Destroy(sliced);
	String_Free(sliced);

	String_Destroy(&s);
}

tsCase(Acute, "Slicing") {
	String s = $("Hello World.");

	String *sliced = String_SafeSlice(&s, 2, 4);
	String_Append(&s, $("Hi.")); /* Valid. Notices that there is a next. */

	Assert($("Equals"), String_Equals(*sliced, $("llo ")));
	Assert($("Equals"), String_Equals(s, $("Hello World.Hi.")));

	String_Destroy(sliced);
	String_Free(sliced);

	String_Destroy(&s);
}

/* Verify that strings which depend on each other don't need to be freed in the
 * same order as they were created. This checks the proper functioning of the
 * doubly linked list.
 */
tsCase(Acute, "Slicing") {
	String s = $("Hello World.");
	String_Append(&s, $("Hi."));

	Assert($("s.prev == NULL"), s.prev == NULL);
	Assert($("s.next == NULL"), s.next == NULL);

	String *sliced = String_SafeSlice(&s, 2, 4);

	/* sliced depends on s */
	Assert($("s.next == &sliced"),    s.next == sliced);
	Assert($("sliced->prev == &s"),   sliced->prev == &s);
	Assert($("sliced->next == NULL"), sliced->next == NULL);

	/* correctly sliced */
	Assert($("Equals"), String_Equals(*sliced, $("llo ")));

	String *sliced2 = String_SafeSlice(sliced, 1, 2);

	/* sliced2 depends on sliced */
	Assert($("s.next == sliced"), s.next == sliced);
	Assert($("sliced->next == sliced2"), sliced->next  == sliced2);
	Assert($("sliced2->prev == sliced"), sliced2->prev == sliced);
	Assert($("sliced2->next == NULL"), sliced2->next == NULL);

	String_Destroy(sliced);
	String_Free(sliced);

	/* now sliced2 depends on s */
	Assert($("s.next == sliced2"), s.next == sliced2);
	Assert($("sliced2->prev == s"), sliced2->prev == &s);

	/* contents still accessible? */
	Assert($("Equals"), String_Equals(*sliced2, $("lo")));

	String_Destroy(sliced2);
	String_Free(sliced2);

	String_Destroy(&s);
}

tsCase(Acute, "Appending and Slicing") {
	/* For this test case to work, we must work with an heap-allocated string.
	 * We also need to append twice to bypass the internal optimizations
	 * Append() performs.
	 */
	String s = String_New(0);

	/* After this call the size is still 0 because it just points to the
	 * rodata string.
	 */
	String_Append(&s, $("Hello World."));

	/* Now it's really heap-allocated. */
	String_Append(&s, $("Hello World."));

	char *oldBuf = s.buf;

	String *sliced = String_SafeSlice(&s, 2, 4);

	Assert($("Equals"), String_Equals(*sliced, $("llo ")));

	/* Will resize s and thus sliced has an invalid pointer. */
	repeat (10) {
		String_Append(&s, $("Hello. "));
	}

	Assert($("Buffer changed"),      oldBuf != s.buf);
	Assert($("`sliced' accessible"), String_Equals(*sliced, $("llo ")));

	String_Crop(&s, 3, 20);

	/* sliced should still contain the same string. */
	Assert($("`sliced' accessible"), String_Equals(*sliced, $("llo ")));

	String_Destroy(sliced);
	String_Free(sliced);

	String_Destroy(&s);
}

tsCase(Acute, "Slicing") {
	String s = String_New(0);
	String_Append(&s, $("Hello World."));
	String_Append(&s, $("Hello World."));

	String *sliced = String_SafeSlice(&s, 2, 4);

	String_Crop(&s, 3, 20);

	Assert($("Equals"), String_Equals(s, $("lo World.Hello World")));
	Assert($("Equals"), String_Equals(*sliced, $("llo ")));

	String_Destroy(sliced);
	String_Free(sliced);

	String_Destroy(&s);
}

tsCase(Acute, "Decoupling") {
	String s = $("Hello World.");

	/* Always call this before you access rodata strings directly. */
	String_Decouple(&s);

	s.buf[0] = 'h';
	s.buf[6] = 'w';

	Assert($("Equals"), String_Equals(s, $("hello world.")));

	String_Destroy(&s);
}

tsCase(Acute, "Slicing") {
	String s = String_New(0);
	String_Append(&s, $("Hello. "));
	String_Append(&s, $("Hello. "));

	String *c1, *c2, *c3;

	c1 = String_SafeClone(&s);
	c2 = String_SafeClone(c1);
	c3 = String_SafeClone(c2);

	Assert($("s.prev == NULL"),  s.prev == NULL);
	Assert($("c3.next == NULL"), c3->next == NULL);

	/* Join c1 and c3. */
	String_Destroy(c2);
	String_Free(c2);

	Assert($("c1->next == c3 && c3->prev == c1"),
		c1->next == c3 &&
		c3->prev == c1);

	/* Join s and c3. */
	String_Destroy(c1);
	String_Free(c1);

	Assert($("s.next == &c3 && c3->prev == &s"),
		s.next   == c3 &&
		c3->prev == &s);

	/* c3 is the new owner. */
	String_Destroy(&s);

	Assert($("Equals"),
		String_Equals(*c3, $("Hello. Hello. ")));

	Assert($("c3->prev == NULL"), c3->prev == NULL);
	Assert($("c3->next == NULL"), c3->next == NULL);

	String_Destroy(c3);
	String_Free(c3);
}

tsCase(Acute, "Slicing") {
	String s = $("Hello World.");

	String *sliced  = String_SafeSlice(&s, 1, 8);
	String *sliced2 = String_SafeSlice(sliced, 2, 4);
	String *sliced3 = String_SafeSlice(sliced2, 0, 2);

	Assert($("Equals"), String_Equals(*sliced3, $("lo")));

	String_Destroy(sliced3);
	String_Destroy(sliced2);
	String_Destroy(sliced);

	String_Free(sliced3);
	String_Free(sliced2);
	String_Free(sliced);

	String_Destroy(&s);
}

tsCase(Acute, "Split") {
	String s = String_New(0);
	String_Append(&s, $("Hello World."));
	String_Append(&s, $("Hello World."));

	Assert($("s.next == NULL"), s.next == NULL);
	StringArray *parts = String_Split(s, ' ');
	Assert($("s.next == NULL"), s.next == NULL);

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

tsCase(Acute, "Cloning") {
	String s = String_New(0);
	String_Append(&s, $("Hello World."));
	String_Append(&s, $("Hello World."));

	Assert($("s.prev == NULL"), s.prev == NULL);
	Assert($("s.next == NULL"), s.next == NULL);

	/* SafeTrim works the same way. */
	String *sliced = String_SafeSlice(&s, 2, 5);

	Assert($("s.prev == NULL"),   s.prev == NULL);
	Assert($("s.next == sliced"), s.next == sliced);

	Assert($("sliced->prev == &s"),   sliced->prev == &s);
	Assert($("sliced->next == NULL"), sliced->next == NULL);

	String *cloned = String_SafeClone(sliced);

	Assert($("sliced->next == cloned"), sliced->next == cloned);
	Assert($("cloned->prev == sliced"), cloned->prev == sliced);

	String_Destroy(sliced);
	String_Free(sliced);

	String_Destroy(&s);

	String_Destroy(cloned);
	String_Free(cloned);
}

tsFinalize;
