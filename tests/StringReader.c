#import <StringReader.h>

#import "TestSuite.h"

#define self tsStringReader

class {

};

tsRegister("StringReader") {
	return true;
}

tsCase(Acute, "Peek") {
	StringReader reader = StringReader_New($("abc"));

	char c1, c2, c3;

	Assert($("Return value"), StringReader_Peek(&reader, &c1));
	Assert($("Return value"), StringReader_Peek(&reader, &c2));
	Assert($("Return value"), StringReader_Peek(&reader, &c3));

	Assert($("Equals"), c1 == 'a');
	Assert($("Equals"), c2 == 'a');
	Assert($("Equals"), c3 == 'a');

	Assert($("Return value"), StringReader_Peek(&reader, &c1, 1));
	Assert($("Return value"), StringReader_Peek(&reader, &c2, 2));
	Assert($("Return value"), !StringReader_Peek(&reader, &c3, 3));

	Assert($("Equals"), c1 == 'b');
	Assert($("Equals"), c2 == 'c');
	Assert($("Equals"), c3 == '\0');
}

tsCase(Acute, "Consume") {
	StringReader reader = StringReader_New($("abc"));

	char c1, c2, c3, c4;

	Assert($("Return value"), StringReader_Peek(&reader, &c1));
	StringReader_Consume(&reader);

	Assert($("Return value"), StringReader_Peek(&reader, &c2));
	StringReader_Consume(&reader);

	Assert($("Return value"), StringReader_Peek(&reader, &c3));
	StringReader_Consume(&reader);

	Assert($("Return value"), !StringReader_Peek(&reader, &c4));

	Assert($("Equals"), c1 == 'a');
	Assert($("Equals"), c2 == 'b');
	Assert($("Equals"), c3 == 'c');
	Assert($("Equals"), c4 == '\0');
}

tsCase(Acute, "Extend") {
	StringReader reader = StringReader_New($("abcde"));

	RdString s = $("");

	StringReader_Consume(&reader);

	StringReader_Extend(&reader, &s);
	Assert($("Equals"), String_Equals(s, $("b")));

	StringReader_Extend(&reader, &s);
	Assert($("Equals"), String_Equals(s, $("bc")));

	StringReader_Extend(&reader, &s);
	Assert($("Equals"), String_Equals(s, $("bcd")));

	s.len = 0;
	StringReader_Extend(&reader, &s);
	Assert($("Equals"), String_Equals(s, $("e")));
}

tsCase(Acute, "GetLine (consume)") {
	StringReader reader = StringReader_New($("ab\ncd\n"));

	Assert($("Equals"), StringReader_GetLine(&reader) == 1);

	StringReader_Consume(&reader); // a
	StringReader_Consume(&reader); // b

	Assert($("Equals"), StringReader_GetLine(&reader) == 1);

	StringReader_Consume(&reader); // \n
	Assert($("Equals"), StringReader_GetLine(&reader) == 2);

	StringReader_Consume(&reader); // c
	Assert($("Equals"), StringReader_GetLine(&reader) == 2);

	StringReader_Consume(&reader); // d
	Assert($("Equals"), StringReader_GetLine(&reader) == 2);

	StringReader_Consume(&reader); // \n
	Assert($("Equals"), StringReader_GetLine(&reader) == 3);
}

tsCase(Acute, "GetLine (extend)") {
	StringReader reader = StringReader_New($("ab\ncd\n"));

	Assert($("Equals"), StringReader_GetLine(&reader) == 1);

	RdString s = $("");

	StringReader_Extend(&reader, &s); // a
	StringReader_Extend(&reader, &s); // b

	Assert($("Equals"), StringReader_GetLine(&reader) == 1);

	StringReader_Extend(&reader, &s); // \n
	Assert($("Equals"), StringReader_GetLine(&reader) == 2);

	StringReader_Extend(&reader, &s); // c
	Assert($("Equals"), StringReader_GetLine(&reader) == 2);

	StringReader_Extend(&reader, &s); // d
	Assert($("Equals"), StringReader_GetLine(&reader) == 2);

	StringReader_Extend(&reader, &s); // \n
	Assert($("Equals"), StringReader_GetLine(&reader) == 3);
}

tsFinalize;
