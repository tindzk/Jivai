#import <String.h>
#import <StringStream.h>

#import "TestSuite.h"

#define self tsStringStream

class {

};

tsRegister("String stream") {
	return true;
}

tsCase(Acute, "Empty string") {
	String s   = $("");
	String out = String_New(16);

	StringStream stream;
	StringStream_Init(&stream, &s);

	Assert($("read()"),
		StringStream_Read(&stream, out.buf, String_GetSize(&out)) == 0);

	Assert($("isEof()"),
		StringStream_IsEof(&stream) == true);

	String_Destroy(&out);
}

tsCase(Acute, "Non-empty string") {
	String s   = $("Hello World.");
	String out = String_New(128);

	StringStream stream;
	StringStream_Init(&stream, &s);

	Assert($("read() return value"),
		(out.len = StringStream_Read(&stream, out.buf, 2)) == 2);

	Assert($("read() contents"),
		String_Equals(out, $("He")));

	Assert($("isEof()"),
		StringStream_IsEof(&stream) == false);

	Assert($("read() return value"),
		(out.len = StringStream_Read(&stream, out.buf, String_GetSize(&out))) == s.len - 2);

	Assert($("read() contents"),
		String_Equals(out, $("llo World.")));

	Assert($("isEof()"),
		StringStream_IsEof(&stream) == true);

	String_Destroy(&out);
}

tsFinalize;
