#import <String.h>
#import <StringStream.h>

#import "TestSuite.h"

#undef self
#define self tsStringStream

class(self) {

};

tsRegister("String stream") {
	return true;
}

tsCase(Acute, "Empty string") {
	String s   = $("");
	String out = StackString(16);

	StringStream stream;
	StringStream_Init(&stream, &s);

	Assert($("read()"),
		StringStream_Read(&stream, out.buf, out.size) == 0);

	Assert($("isEof()"),
		StringStream_IsEof(&stream) == true);
}

tsCase(Acute, "Non-empty string") {
	String s   = $("Hello World.");
	String out = StackString(128);

	StringStream stream;
	StringStream_Init(&stream, &s);

	Assert($("read() return value"),
		(out.len = StringStream_Read(&stream, out.buf, 2)) == 2);

	Assert($("read() contents"),
		String_Equals(out, $("He")));

	Assert($("isEof()"),
		StringStream_IsEof(&stream) == false);

	Assert($("read() return value"),
		(out.len = StringStream_Read(&stream, out.buf, out.size)) == s.len - 2);

	Assert($("read() contents"),
		String_Equals(out, $("llo World.")));

	Assert($("isEof()"),
		StringStream_IsEof(&stream) == true);
}

tsFinalize;
