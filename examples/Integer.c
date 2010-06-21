#include <Integer.h>
#include <String.h>

ExceptionManager exc;

int main(void) {
	ExceptionManager_Init(&exc);

	String0(&exc);
	Integer0(&exc);

	String_Print(Integer_ToString(-123456789));
	String_Print(String("\n"));

	String_Print(Integer_ToString(123456789));
	String_Print(String("\n"));

	String_Print(Integer64_ToString(-12345678987654321));
	String_Print(String("\n"));

	String_Print(Integer64_ToString(12345678987654321));
	String_Print(String("\n"));

	return EXIT_SUCCESS;
}
