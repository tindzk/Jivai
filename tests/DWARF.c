#import <ELF.h>
#import <DWARF.h>

#import "TestSuite.h"

#define self tsDWARF

class {

};

tsRegister("DWARF") {
	return true;
}

def(void *, GetAddr) {
	return __builtin_return_address(0);
}

tsCase(Acute, "Line number program") {
	bool error = false;

	try {
		ELF elf = ELF_New($("./TestSuite.bin"));

		DWARF dwarf = DWARF_New(ELF_GetSection(&elf, $(".debug_line")));
		DWARF_ParseLineNumberProgram(&dwarf);

		DWARF_Match match = DWARF_ResolveSymbol(&dwarf, call(GetAddr)); u32 line = __LINE__;

		Assert($("Line number"), match.line == line);
		Assert($("File name"),   String_Equals(match.file.name, $("DWARF.c")));

		DWARF_Destroy(&dwarf);
		ELF_Destroy(&elf);
	} catchAny {
		error = true;
	} finally {

	} tryEnd;

	Assert($("No error"), !error);
}
