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
		String_GetSize(this->s) >= 16);

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
	RdString append = $("Test.");

	size_t caught = false;

	try {
		String_Append(&s, append);
	} catch(String, DoubleFree) {
		caught = true;
	} finally {
	} tryEnd;

	Assert($("Heap strings are mutable"), !caught);

	String_Destroy(&s);
}

tsCase(Acute, "Appending (3)") {
	String s = String_New(1);
	String_Append(&s, $("Hello World."));

	Assert($("Not empty"), s.len > 0);
	Assert($("Equals"), String_Equals(s.rd, $("Hello World.")));

	String_Destroy(&s);
}

tsCase(Acute, "Appending (4)") {
	String s = String_New(16);
	String_Append(&s, $("Hello World."));

	Assert($("Not empty"), s.len > 0);
	Assert($("Equals"), String_Equals(s.rd, $("Hello World.")));

	String_Destroy(&s);
}

tsCase(Acute, "Resizing") {
	String s = String_New(1);
	String_Append(&s, 'a');

	String_Resize(&s, 0);

	Assert($("Length is 0"),
		s.len == 0);

	Assert($("Buffer not NULL"),
		s.buf != NULL);

	String_Destroy(&s);

	Assert($("Empty buffer"),
		s.buf == (void *) 0xdeadbeef);
}

tsCase(Acute, "Cloning") {
	RdString s = $("Hello World.");

	String cloned = String_Clone(s);

	Assert($("Don't inherit rodata pointer"),
		cloned.buf != s.buf);

	String_Destroy(&cloned);

	Assert($("Original string still accessible"),
		String_Equals(s, $("Hello World.")));
}

tsCase(Acute, "Copying") {
	String s = String_New(0);

	Assert($("Empty buffer"),
		s.buf == NULL);

	RdString copy = $("Hello World.");

	/* Multiple calls to check whether there are any leaks. */
	String_Copy(&s, copy);
	String_Copy(&s, copy);
	String_Copy(&s, copy);

	Assert($("Has correct length"),
		s.len == copy.len);
	Assert($("Has correct size"),
		String_GetSize(s) >= copy.len);

	String_Destroy(&s);
}

tsCase(Acute, "Copying") {
	String s = String_New(0);
	String_Append(&s, $("Hello World."));
	String_Append(&s, $("Hello World."));

	Assert($("Heap-allocated"),
		!Memory_IsRoData(s.buf));

	RdString cut = String_Slice(s.rd, 1, 5);

	String dest = String_New(0);
	String_Copy(&dest, cut);

	Assert($("Does not point to copy"),
		dest.buf != cut.buf);

	String_Destroy(&dest);
	String_Destroy(&s);
}

tsCase(Acute, "Copying") {
	String s = String_New(1);

	RdString copy = $("Hello World.");

	String_Copy(&s, copy);
	String_Copy(&s, copy);
	String_Copy(&s, copy);

	Assert($("Does not point to copy"),
		s.buf != copy.buf);
	Assert($("Is resized"),
		String_GetSize(s) >= copy.len);
	Assert($("Has correct length"),
		s.len == copy.len);

	String_Destroy(&s);
}

tsCase(Acute, "Copying") {
	String s = String_New(1);

	String copy = String_New(1);
	String_Append(&copy, $("Hello World."));

	String_Copy(&s, copy.rd);
	String_Copy(&s, copy.rd);
	String_Copy(&s, copy.rd);

	Assert($("Does not point to copy"),
		s.buf != copy.buf);
	Assert($("Is resized"),
		String_GetSize(s) >= copy.len);
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
	} catch(String, ElementMismatch) {
		caught = true;
	} finally {
	} tryEnd;

	Assert($("Detected mismatch"), caught);

	String_Destroy(&s);
}

tsCase(Acute, "Parsing") {
	RdString subject  = $("Hello World.");
	RdString pattern  = $("Hello %.");
	RdString expected = $("World");

	RdString result;
	bool matches = String_Parse(pattern, subject, &result);

	Assert($("Matches"), matches);
	Assert($("Equals"),  String_Equals(result, expected));
}

tsCase(Acute, "Parsing") {
	RdString subject   = $("Hello World.");
	RdString pattern   = $("% %");
	RdString expected  = $("Hello");
	RdString expected2 = $("World.");

	RdString result, result2;
	bool matches = String_Parse(pattern, subject, &result, &result2);

	Assert($("Matches"), matches);
	Assert($("Equals"),  String_Equals(result,  expected));
	Assert($("Equals"),  String_Equals(result2, expected2));
}

tsCase(Acute, "Parsing") {
	RdString subject   = $(" Hello  World.");
	RdString pattern   = $("%  %");
	RdString expected  = $(" Hello");
	RdString expected2 = $("World.");

	RdString result, result2;
	bool matches = String_Parse(pattern, subject, &result, &result2);

	Assert($("Matches"), matches);
	Assert($("Equals"),  String_Equals(result,  expected));
	Assert($("Equals"),  String_Equals(result2, expected2));
}

tsCase(Acute, "Parsing") {
	RdString subject   = $(".value");
	RdString pattern   = $("%.%");
	RdString expected  = $("");
	RdString expected2 = $("value");

	RdString result, result2;
	bool matches = String_Parse(pattern, subject, &result, &result2);

	Assert($("Matches"), matches);
	Assert($("Equals"),  String_Equals(result,  expected));
	Assert($("Equals"),  String_Equals(result2, expected2));
}

tsCase(Acute, "Parsing") {
	RdString subject  = $("");
	RdString pattern  = $("%");
	RdString expected = $("");

	RdString result;
	bool matches = String_Parse(pattern, subject, &result);

	Assert($("Matches"), matches);
	Assert($("Equals"),  String_Equals(result, expected));
}

tsCase(Acute, "Parsing") {
	RdString subject  = $("");
	RdString pattern  = $("a%");

	RdString result;
	bool matches = String_Parse(pattern, subject, &result);

	Assert($("Matches"), !matches);
}

tsCase(Acute, "Format") {
	String s = String_Format($("Hel!%lo %."), $("World"));

	RdString expected = $("Hel%lo World.");

	/* Make sure String_Format() doesn't allocate too much. */
	Assert($("Size"),   String_GetSize(s) >= expected.len);
	Assert($("Equals"), String_Equals(s.rd, expected));

	String_Destroy(&s);
}

tsCase(Acute, "Appending") {
	String s = String_New(0);

	String_Copy(&s, $("Hello "));
	String_Append(&s, $("World."));

	Assert($("Equals"), String_Equals(s.rd, $("Hello World.")));

	String_Destroy(&s);
}

tsCase(Acute, "Destruction of cloned rodata strings") {
	String s = String_Clone($("Hello World."));
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
	} catch(String, DoubleFree) {
		caught = true;
	} finally {

	} tryEnd;

	Assert($("Caught"), caught);
}

/* Append to sliced. */
tsCase(Acute, "Slicing") {
	RdString s = $("Hello World.");

	String sliced = String_Clone(String_Slice(s, 2, 4));
	String_Append(&sliced, $("Hi."));
	Assert($("Equals"), String_Equals(sliced.rd, $("llo Hi.")));
	String_Destroy(&sliced);
}

tsCase(Acute, "Trim") {
	RdString subject  = $("  Hello World  \n");
	RdString expected = $("Hello World");

	Assert($("Result"),
		String_Equals(
			String_Trim(subject),
			expected));
}

tsCase(Acute, "Split") {
	String s = String_New(0);
	String_Append(&s, $("Hello World."));
	String_Append(&s, $("Hello World."));

	RdStringArray *parts = String_Split(s.rd, ' ');

	String_Destroy(&s);

	RdStringArray_Free(parts);
}

tsCase(Acute, "Split") {
	String s = String_New(0);
	String_Append(&s, $("Hello World."));
	String_Append(&s, $("Hello World."));

	RdString part = $("");
	Assert($("Result"), String_Split(s.rd, ' ', &part));
	Assert($("off=0 len=5"), part.buf == s.buf && part.len == 5);

	Assert($("Result"), String_Split(s.rd, ' ', &part));
	Assert($("off=6 len=11"), part.buf == s.buf + 6 && part.len == 11);

	Assert($("Result"), String_Split(s.rd, ' ', &part));
	Assert($("off=18 len=6"), part.buf == s.buf + 18 && part.len == 6);

	String_Destroy(&s);
}

tsCase(Acute, "Split with empty string") {
	RdString part = $("");
	Assert($("Result"), String_Split($(""), ' ', &part) == false);
}

tsCase(Acute, "Split with empty string") {
	RdString part = $("");
	Assert($("Result"), String_Split($(""), $(" "), &part) == false);
}

tsCase(Acute, "Split") {
	RdString part    = $("");
	RdString subject = $("This  is  a  sentence.");

	Assert($("Result"), String_Split(subject, $("  "), &part) == true);
	Assert($("Equals"), String_Equals(part, $("This")));
	Assert($("Result"), String_Split(subject, $("  "), &part) == true);
	Assert($("Equals"), String_Equals(part, $("is")));
	Assert($("Result"), String_Split(subject, $("  "), &part) == true);
	Assert($("Equals"), String_Equals(part, $("a")));
	Assert($("Result"), String_Split(subject, $("  "), &part) == true);
	Assert($("Equals"), String_Equals(part, $("sentence.")));
	Assert($("Result"), String_Split(subject, $("  "), &part) == false);
}

tsCase(Acute, "Joining") {
	RdString orig  = $("");
	RdString orig2 = $(" ");
	RdString orig3 = $("abc ab a");
	RdString orig4 = $("abc ab a ");

	RdStringArray *items  = String_Split(orig,  ' ');
	RdStringArray *items2 = String_Split(orig2, ' ');
	RdStringArray *items3 = String_Split(orig3, ' ');
	RdStringArray *items4 = String_Split(orig4, ' ');

	String joined  = RdStringArray_Join(items,  $(" "));
	String joined2 = RdStringArray_Join(items2, $(" "));
	String joined3 = RdStringArray_Join(items3, $(" "));
	String joined4 = RdStringArray_Join(items4, $(" "));

	Assert($("Empty"),         String_Equals(orig,  joined.rd));
	Assert($("Non-empty"),     String_Equals(orig2, joined2.rd));
	Assert($("Non-empty (2)"), String_Equals(orig3, joined3.rd));
	Assert($("Non-empty (3)"), String_Equals(orig4, joined4.rd));

	RdStringArray_Free(items4);
	RdStringArray_Free(items3);
	RdStringArray_Free(items2);
	RdStringArray_Free(items);

	String_Destroy(&joined4);
	String_Destroy(&joined3);
	String_Destroy(&joined2);
	String_Destroy(&joined);
}

tsCase(Acute, "Begins with") {
	RdString str = $("Hello World.");

	Assert($("Result"), String_BeginsWith(str, $("")));
	Assert($("Result"), String_BeginsWith(str, $("H")));
	Assert($("Result"), String_BeginsWith(str, $("Hello")));
	Assert($("Result"), String_BeginsWith(str, $("Hello World.")));
	Assert($("Result"), !String_BeginsWith(str, $("e")));
	Assert($("Result"), !String_BeginsWith(str, $("ello World.")));
}

tsCase(Acute, "Ends with") {
	RdString str = $("Hello World.");

	Assert($("Result"), String_EndsWith(str, $("")));
	Assert($("Result"), String_EndsWith(str, $(".")));
	Assert($("Result"), String_EndsWith(str, $("World.")));
	Assert($("Result"), String_EndsWith(str, $("Hello World.")));
	Assert($("Result"), !String_EndsWith(str, $("d")));
	Assert($("Result"), !String_EndsWith(str, $("Hello World")));
}

tsCase(Acute, "Find") {
	RdString subject = $("Hello World.");
	RdString needle  = $("Hello World.");
	RdString needle2 = $("World");
	RdString needle3 = $("Hello World...");
	RdString needle4 = $("Hello World!");
	char needle5 = '.';
	char needle6 = 'l';

	Assert($("Result"), String_Find(subject, needle)  == 0);
	Assert($("Result"), String_Find(subject, needle2) == 6);
	Assert($("Result"), String_Find(subject, needle3) == String_NotFound);
	Assert($("Result"), String_Find(subject, needle4) == String_NotFound);
	Assert($("Result"), String_Find(subject, needle5) == 11);
	Assert($("Result"), String_Find(subject, needle6) == 2);
}

tsCase(Acute, "Reverse find") {
	RdString subject = $("Hello World.");
	RdString needle  = $("Hello World.");
	RdString needle2 = $("World");
	RdString needle3 = $("Hello World...");
	RdString needle4 = $("Hello World!");
	char needle5 = '.';
	char needle6 = 'l';

	Assert($("Result"), String_ReverseFind(subject, needle)  == 0);
	Assert($("Result"), String_ReverseFind(subject, needle2) == 6);
	Assert($("Result"), String_ReverseFind(subject, needle3) == String_NotFound);
	Assert($("Result"), String_ReverseFind(subject, needle4) == String_NotFound);
	Assert($("Result"), String_ReverseFind(subject, needle5) == 11);
	Assert($("Result"), String_ReverseFind(subject, needle6) == 9);
}

tsCase(Acute, "Replace") {
	RdString s = $("Hello World");

	CarrierString result = String_Replace(s, $(" Hello"), $(""));
	Assert($("Stack"),    result.omni);
	Assert($("Original"), result.buf == s.buf && result.len == s.len);
	CarrierString_Destroy(&result);

	result = String_Replace(s, $("Hello World"), $(""));
	Assert($("Heap"),   !result.omni);
	Assert($("Equals"), String_Equals(result.rd, $("")));
	Assert($("No allocation"), result.buf == NULL && result.len == 0);
	CarrierString_Destroy(&result);

	result = String_Replace($(""), $(""), $(""));
	Assert($("Stack"),  result.omni);
	Assert($("Equals"), String_Equals(result.rd, $("")));
	CarrierString_Destroy(&result);

	result = String_Replace($("Hello"), $("Hello"), $(""));
	Assert($("Heap"),   !result.omni);
	Assert($("Equals"), String_Equals(result.rd, $("")));
	CarrierString_Destroy(&result);

	result = String_Replace(s, $("Hello"), $("Hi"));
	Assert($("Heap"),   !result.omni);
	Assert($("Equals"), String_Equals(result.rd, $("Hi World")));
	CarrierString_Destroy(&result);
}

tsCase(Acute, "Replace all") {
	RdString s = $("Hello World");

	CarrierString result = String_ReplaceAll(s, $(" Hello"), $(""));
	Assert($("Stack"),    result.omni);
	Assert($("Original"), result.buf == s.buf && result.len == s.len);
	CarrierString_Destroy(&result);

	result = String_ReplaceAll(s, $("Hello World!"), $(""));
	Assert($("Stack"),    result.omni);
	Assert($("Original"), result.buf == s.buf && result.len == s.len);
	CarrierString_Destroy(&result);

	result = String_ReplaceAll(s, $("o"), $("*"));
	Assert($("Heap"),   !result.omni);
	Assert($("Equals"), String_Equals(result.rd, $("Hell* W*rld")));
	CarrierString_Destroy(&result);
}

tsCase(Acute, "Between") {
	RdString s = $("Hello World");

	RdString result = $("");

	Assert($("Result"), String_Between(s, $("H"), $("d"), &result));
	Assert($("Equals"), String_Equals(result, $("ello Worl")));

	result = $("");

	Assert($("Result"), !String_Between(s, $("H"), $("World!"), &result));
	Assert($("Equals"), String_Equals(result, $("")));
}

tsCase(Acute, "Between (2)") {
	RdString s = $("<a><b><c>");
	RdString result = $("");

	Assert($("Result"), String_Between(s, $("<"), $(">"), &result));
	Assert($("Equals"), String_Equals(result, $("a")));

	Assert($("Result"), String_Between(s, $("<"), $(">"), &result));
	Assert($("Equals"), String_Equals(result, $("b")));

	Assert($("Result"), String_Between(s, $("<"), $(">"), &result));
	Assert($("Equals"), String_Equals(result, $("c")));

	Assert($("Result"), !String_Between(s, $("<"), $(">"), &result));
	Assert($("Equals"), String_Equals(result, $("")));
}

tsCase(Acute, "Between (3)") {
	RdString s = $("<><><>");
	RdString result = $("");

	Assert($("Result"), String_Between(s, $("<"), $(">"), &result));
	Assert($("Equals"), String_Equals(result, $("")));

	Assert($("Result"), String_Between(s, $("<"), $(">"), &result));
	Assert($("Equals"), String_Equals(result, $("")));

	Assert($("Result"), String_Between(s, $("<"), $(">"), &result));
	Assert($("Equals"), String_Equals(result, $("")));

	Assert($("Result"), !String_Between(s, $("<"), $(">"), &result));
	Assert($("Equals"), String_Equals(result, $("")));
}

tsCase(Acute, "Cropping") {
	String s = String_New(0);
	String_Crop(&s, 0);
	String_Destroy(&s);

	s = String_Clone($("Hello."));

	String_Crop(&s, 0);
	Assert($("Equals"), String_Equals(s.rd, $("Hello.")));

	String_Crop(&s, 1);
	Assert($("Equals"), String_Equals(s.rd, $("ello.")));

	String_Crop(&s, 5);
	Assert($("Equals"), String_Equals(s.rd, $("")));

	String_Destroy(&s);
}
