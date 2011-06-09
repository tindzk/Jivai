#import "DWARF.h"

#define self DWARF

rsdef(self, New, RdBuffer buf) {
	assert(buf.len != 0);

	return (self) {
		.buf = buf,
		.matches = scall(Matches_New, 128)
	};
}

def(void, Destroy) {
	scall(Matches_Free, this->matches);
}

static def(void, AddMatch, ref(Files) *files, u32 address, u32 file, u32 line) {
	assert(file != 0);
	assert(file - 1 < files->len);

	scall(Matches_Push, &this->matches, (ref(Match)) {
		.address = address,
		.file    = files->buf[file - 1],
		.line    = line
	});
}

static def(void *, AddFile, RdStringArray *paths, ref(Files) *files, void *ptr) {
	RdString name = String_FromNul(ptr);
	ptr += name.len + 1;

	/* `dirndx' is 0 when the source file was found in current
	 * directory during compilation.
	 * Otherwise the value can be found in the directory table
	 * under the index dirndx-1.
	 */
	u32 dirndx;
	ptr += LEB128_ReadUnsigned(ptr, &dirndx);

	/* optional; is 0 when unspecified. */
	u32 mtime;
	ptr += LEB128_ReadUnsigned(ptr, &mtime);

	/* optional; is 0 when unspecified. */
	u32 fsize;
	ptr += LEB128_ReadUnsigned(ptr, &fsize);

	RdString path = $("");

	if (dirndx != 0) {
		assert(dirndx - 1 < paths->len);
		path = paths->buf[dirndx - 1];
	}

	scall(Files_Push, &files, (ref(File)) {
		.path = path,
		.name = name
	});

	return ptr;
}

static def(void, StateMachine, RdStringArray *paths, ref(Files) *files, void *p, ref(LineTableHeader) *hdr) {
	u32 file = 1;
	u32 line = 1;
	ref(Pointer) address = 0;

	/* The first opcode must be an extended opcode setting the initial address.
	 *
	 * TODO This doesn't seem to be valid for 64-bit binaries compiled
	 * with Clang where in some cases the first opcode sets the column
	 * to 0.
	 */
	// assert(*(ubyte *) p == 0);

	int addrAdvance =
		((255 - hdr->opcode_base) / hdr->line_range * hdr->minimum_instruction_length);

	void *cur = p;
	while ((void *) cur < p + hdr->header_length) {
		ubyte opcode = *(ubyte *) cur;
		cur++;

		if (opcode == DW_LNS_extended_op) {
			/* Extended opcode. */
			u32 opsize;
			cur += LEB128_ReadUnsigned(cur, &opsize);

			/* `opsize' already includes one byte for `opcode'. */
			assert(opsize > 0);
			opsize--;

			opcode = *(ubyte *) cur;
			cur++;

			if (opcode == DW_LNE_end_sequence) {
				call(AddMatch, files, address, file, line);
				break;
			} else if (opcode == DW_LNE_set_address) {
				address = *(ref(Pointer) *) cur;
				assert(opsize == sizeof(ref(Pointer)));
			} else if (opcode == DW_LNE_define_file) {
				cur = call(AddFile, paths, files, cur);
			} else {
				/* Unrecognized extended opcode. */
				assert(0);
			}

			cur += opsize;
		} else if (opcode > 0 && opcode < hdr->opcode_base) {
			/* Standard opcode. */
			if (opcode == DW_LNS_copy) {
				call(AddMatch, files, address, file, line);
			} else if (opcode == DW_LNS_advance_pc) {
				u32 delta;
				cur += LEB128_ReadUnsigned(cur, &delta);

				address += delta * hdr->minimum_instruction_length;
			} else if (opcode == DW_LNS_advance_line) {
				s32 delta;
				cur += LEB128_ReadSigned(cur, &delta);

				line += delta;
			} else if (opcode == DW_LNS_set_file) {
				cur += LEB128_ReadUnsigned(cur, &file);
			} else if (opcode == DW_LNS_set_column) {
				/* Ignore. */
				u32 column;
				cur += LEB128_ReadUnsigned(cur, &column);
			} else if (opcode == DW_LNS_negate_stmt) {
				/* Ignore. */
			} else if (opcode == DW_LNS_set_basic_block) {
				/* Ignore. */
			} else if (opcode == DW_LNS_const_add_pc) {
				address += addrAdvance;
			} else if (opcode == DW_LNS_fixed_advance_pc) {
				uhalf delta = *(uhalf *) cur;
				address += delta;
			} else if (opcode == DW_LNS_set_prologue_end) {
				/* Ignore. */
			} else if (opcode == DW_LNS_set_epilogue_begin) {
				/* Ignore. */
			} else if (opcode == DW_LNS_set_isa) {
				/* Ignore. */
				u32 isa;
				cur += LEB128_ReadUnsigned(cur, &isa);
			} else {
				/* Unrecognized extended opcode. */
				assert(0);
			}
		} else {
			/* Special opcode. */
			s32 delta = opcode - hdr->opcode_base;

			line    += hdr->line_base + (delta % hdr->line_range);
			address += (delta / hdr->line_range) * hdr->minimum_instruction_length;

			call(AddMatch, files, address, file, line);
		}
	}
}

def(void, ParseLineNumberProgram) {
	ref(LineTableHeader) *cur = this->buf.ptr;

	while ((void *) cur < this->buf.ptr + this->buf.len) {
#if DWARF_Version == 3 && DWARF_64bit
		/* 0xffffffff = -1 */
		assert(cur->total_length0 == 0xffffffff);

		assert(this->buf.len > cur->total_length +
			sizeof(cur->total_length0) +
			sizeof(cur->total_length));
#else
		/* There is a typo in the DWARF 3 specification (Dec 20, 2005).
		 * It says 0xffffff00 whereas the correct value is 0xfffffff0.
		 * This was fixed in DWARF 4.
		 */
		assert(cur->total_length < 0xfffffff0);

		assert(this->buf.len >
			cur->total_length + sizeof(cur->total_length));

		assert(cur->header_length < 0xfffffff0);
#endif

		assert(cur->version == DWARF_Version);
		assert(cur->header_length < cur->total_length);

		ubyte *ptr = &cur->standard_opcode_lengths[0];

		/* Skip opcode lengths. */
		rpt(cur->opcode_base - 1) {
			u32 res = 0;
			ptr += LEB128_ReadUnsigned(ptr, &res);
		}

		RdStringArray *paths = RdStringArray_New(64);

		/* Read include directory table. */
		while (*ptr != '\0') {
			RdString path = String_FromNul(ptr);
			RdStringArray_Push(&paths, path);
			ptr += path.len + 1;
		}

		ptr++;

		/* Read file name table. */
		ref(Files) *files = scall(Files_New, 128);
		while (*ptr != '\0') {
			ptr = call(AddFile, paths, files, ptr);
		}

		ptr++;

		call(StateMachine, paths, files, ptr, cur);

		scall(Files_Free, files);
		RdStringArray_Free(paths);

		cur = (void *) cur + sizeof(cur->total_length) + cur->total_length;
	}
}

def(ref(Match), ResolveSymbol, void *addr) {
	fwd(i, this->matches->len) {
		if (i + 1 < this->matches->len) {
			if (addr >= this->matches->buf[i + 1].address) {
				continue;
			}
		}

		if (addr >= this->matches->buf[i].address) {
			return this->matches->buf[i];
		}
	}

	return (ref(Match)) { .address = NULL };
}
