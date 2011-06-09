#import <Char.h>

#import "TestSuite.h"

#define self tsChar

class {
	String s;
};

tsRegister("Char") {
	return true;
}

tsCase(Acute, "Alpha") {
	Assert($("a"),  Char_IsAlpha('a'));
	Assert($("0"), !Char_IsAlpha('0'));
	Assert($("?"), !Char_IsAlpha('?'));
}

tsCase(Acute, "Lower") {
	Assert($("a"),  Char_IsLower('a'));
	Assert($("A"), !Char_IsLower('A'));
	Assert($("?"), !Char_IsLower('?'));

	Assert($("a"), Char_ToLower('a') == 'a');
	Assert($("A"), Char_ToLower('A') == 'a');
	Assert($("?"), Char_ToLower('?') == '?');
}

tsCase(Acute, "Upper") {
	Assert($("a"), !Char_IsUpper('a'));
	Assert($("A"),  Char_IsUpper('A'));
	Assert($("?"), !Char_IsUpper('?'));

	Assert($("a"), Char_ToUpper('a') == 'A');
	Assert($("A"), Char_ToUpper('A') == 'A');
	Assert($("?"), Char_ToUpper('?') == '?');
}

tsCase(Acute, "Digit") {
	Assert($("a"), !Char_IsDigit('a'));
	Assert($("0"),  Char_IsDigit('0'));
	Assert($("?"), !Char_IsDigit('A'));

	Assert($("0"), Char_ParseDigit('0') == 0);
	Assert($("9"), Char_ParseDigit('9') == 9);
}

tsCase(Acute, "Printable") {
	Assert($("' '"),  Char_IsPrintable(' '));
	Assert($("a"),    Char_IsPrintable('a'));
	Assert($("*"),    Char_IsPrintable('*'));
	Assert($("\\n"), !Char_IsPrintable('\n'));
	Assert($("\\t"), !Char_IsPrintable('\t'));
	Assert($("\\0"), !Char_IsPrintable('\0'));
}
