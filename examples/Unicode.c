#import <String.h>
#import <Unicode.h>
#import <Integer.h>

void printChars(String s) {
	size_t offset = 0;

	while (true) {
		String tmp = StackString(0);

		tmp.buf = s.buf + offset;
		tmp.len = Unicode_Next(s, offset);

		if (tmp.len == 0) {
			break;
		}

		String_FmtPrint($("% (%) "),
			tmp, Int16_ToString(tmp.len));

		offset += tmp.len;
	}
}

void printCharsReverse(String s) {
	size_t offset = s.len;

	while (true) {
		String tmp = StackString(0);
		tmp.len = Unicode_Prev(s, offset);

		if (tmp.len == 0) {
			break;
		}

		offset -= tmp.len;
		tmp.buf = s.buf + offset;

		String_FmtPrint($("% (%) "),
			tmp, Int16_ToString(tmp.len));
	}
}

int main(void) {
	String s;

	s = String("ßühiöö");

	String_FmtPrint($("len=%\n"),
		Int16_ToString(Unicode_Count(s)));

	printChars(s);
	String_Print(String("\n"));
	printCharsReverse(s);

	s = String("\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e");

	String_Print(String("\n\n"));

	String_FmtPrint($("len=%\n"),
		Int16_ToString(Unicode_Count(s)));

	printChars(s);
	String_Print(String("\n"));
	printCharsReverse(s);

	String_Print(String("\n"));

	return ExitStatus_Success;
}
