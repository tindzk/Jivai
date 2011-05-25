#import <ELF.h>

#import "TestSuite.h"

#define self tsELF

class {

};

tsRegister("ELF") {
	return true;
}

tsCase(Acute, "Validation") {
	bool error = false;

	try {
		ELF elf = ELF_New($("./TestSuite.bin"));
		ELF_Destroy(&elf);
	} catchAny {
		error = true;
	} finally {

	} tryEnd;

	Assert($("Valid file"), !error);
}

tsCase(Acute, "Symbol table") {
	ELF elf = ELF_New($("./TestSuite.bin"));

	ELF_Symbols *symbols = ELF_GetSymbolTable(&elf);

	/* Try a function pointer. */
	RdString res = ELF_Symbols_FindNearest(symbols, ELF_New);
	Assert($("Result"), String_Equals(res, $("ELF_New")));

	/* Try a return address. Should return the function which
	 * called this method.
	 */
	res = ELF_Symbols_FindNearest(symbols, __builtin_return_address(0));
	Assert($("Result"), String_Equals(res, $("TestSuite_RunTestSuite")));

	ELF_Symbols_Free(symbols);

	ELF_Destroy(&elf);
}

tsFinalize;
