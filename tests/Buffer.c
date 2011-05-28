#import <Buffer.h>

#import "TestSuite.h"

#define self tsBuffer

class {

};

tsRegister("Buffer") {
	return true;
}

tsCase(Acute, "Allocate") {
	Buffer buf = Buffer_New(20);

	Assert($("Length"),  buf.len == 0);
	Assert($("Pointer"), buf.ptr != NULL);

	Buffer_Destroy(&buf);
}

tsCase(Acute, "Copy") {
	String s   = String_Clone($("Hello World."));
	Buffer buf = Buffer_New(16);

	RdBuffer src  = String_GetRdBuffer(s.rd);
	WrBuffer dest = Buffer_AsWrBuffer(&buf);

	Buffer_Move(dest, src);

	buf.len = s.len;

	Assert($("Equals"), Buffer_Equals(buf.rd, src));

	Buffer_Destroy(&buf);
	String_Destroy(&s);
}
