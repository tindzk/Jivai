#import <dwarf.h>

#import "Buffer.h"
#import "String.h"
#import "LEB128.h"
#import "Exception.h"

#define self DWARF

#define DW_LNS_extended_op 0

record(ref(LineTableHeader)) {
	uword total_length;
	uhalf version;
	uword header_length;
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
	void *address;
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
def(ref(Match), ResolveSymbol, void *addr);

#undef self
