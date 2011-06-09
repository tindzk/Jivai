#import <dwarf.h>

#import "Buffer.h"
#import "String.h"
#import "LEB128.h"
#import "Exception.h"

#define self DWARF

/* Only version 2 and 3 are supported. */
#ifndef DWARF_Version
	#define DWARF_Version 2
#endif

#ifndef DWARF_64bit
	#define DWARF_64bit defined(__x86_64__)
#endif

#define DW_LNS_extended_op 0

#if DWARF_64bit
	typedef s64 ref(Pointer);
#else
	typedef s32 ref(Pointer);
#endif

record(ref(LineTableHeader)) {
#if DWARF_Version == 3 && DWARF_64bit
	u32 total_length0;
	u64 total_length;
#else
	u32 total_length;
#endif

	u16 version;

#if DWARF_Version == 3 && DWARF_64bit
	u64 header_length;
#else
	u32 header_length;
#endif

	ubyte minimum_instruction_length;
	ubyte default_is_stmt;
	byte  line_base;
	ubyte line_range;
	ubyte opcode_base;
	ubyte standard_opcode_lengths[0];

	/* `standard_opcode_lengths' as well as the following two parameters
	 * can only be read dynamically:
	 *  - include_directories
	 *  - file_names
	 */
} __packed;

record(ref(File)) {
	RdString path;
	RdString name;
};

Array(ref(File), ref(Files));

record(ref(Match)) {
	ref(Pointer) address;
	ref(File) file;
	u32 line;
};

Array(ref(Match), ref(Matches));

class {
	RdBuffer buf;
	ref(Matches) *matches;
};

rsdef(self, New, RdBuffer buf);
def(void, Destroy);
def(void, ParseLineNumberProgram);
def(ref(Match), ResolveSymbol, ref(Pointer) addr);

#undef self
