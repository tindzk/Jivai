#import <String.h>
#import <Integer.h>

int main(void) {
	String_Print(Int32_ToString(-123456789));
	String_Print($("\n"));

	String_Print(Int32_ToString(123456789));
	String_Print($("\n"));

	String_Print(Int64_ToString(-12345678987654321));
	String_Print($("\n"));

	String_Print(Int64_ToString(12345678987654321));
	String_Print($("\n"));

	return ExitStatus_Success;
}
