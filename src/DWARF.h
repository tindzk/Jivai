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

/* DWARF's (DW) line number (LN) program constants. */
enum {
	/* Standard opcodes (S). */
	DW_LNS_copy               = 1,
	DW_LNS_advance_pc         = 2,
	DW_LNS_advance_line       = 3,
	DW_LNS_set_file           = 4,
	DW_LNS_set_column         = 5,
	DW_LNS_negate_stmt        = 6,
	DW_LNS_set_basic_block    = 7,
	DW_LNS_const_add_pc       = 8,
	DW_LNS_fixed_advance_pc   = 9,
	DW_LNS_set_prologue_end   = 10,
	DW_LNS_set_epilogue_begin = 11,
	DW_LNS_set_isa            = 12,

	/* Extended opcodes (E). */
	DW_LNE              = 0,
	DW_LNE_end_sequence = 1,
	DW_LNE_set_address  = 2,
	DW_LNE_define_file  = 3
};

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
