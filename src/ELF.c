#import "ELF.h"

#define self ELF

static const char magicSequence[] = {
	0x7f, 'E', 'L', 'F'
};

rsdef(self, New, RdString path) {
	MemoryMappedFile file = MemoryMappedFile_new(path);

	ref(Ehdr) *header = MemoryMappedFile_getAddress(&file);

	/* Header must match magic sequence, otherwise it's not a valid ELF file. */
	if (!Memory_Equals(header->e_ident, (char *) magicSequence, sizeof(magicSequence))) {
		throw(InvalidFile);
	}

	if (header->e_ident[EI_CLASS] != ref(Class)) {
		/* Wrong architecture. */
		throw(InvalidFile);
	}

	/* Validate some fields. */
	assert(header->e_phoff != 0);
	assert(header->e_shoff != 0);
	assert(header->e_shstrndx < header->e_shnum);
	assert(header->e_shstrndx != SHN_UNDEF);

	return (self) {
		.file = file,
		.base = MemoryMappedFile_getAddress(&file)
	};
}

def(void, Destroy) {
	MemoryMappedFile_destroy(&this->file);
}

static def(RdString, ResolveSectName, size_t ofs) {
	ref(Ehdr) *header   = this->base;
	ref(Shdr) *section  = this->base + header->e_shoff;
	ref(Shdr) *shstrtab = &section[header->e_shstrndx];

	assert(ofs < shstrtab->sh_size);
	char *data = this->base + shstrtab->sh_offset;

	return String_FromNul(data + ofs);
}

overload def(RdBuffer, GetSection, RdString name) {
	assert(name.len != 0);

	ref(Ehdr) *header  = this->base;
	ref(Shdr) *section = this->base + header->e_shoff;

	fwd(i, header->e_shnum) {
		ref(Shdr) *cur = &section[i];

		if (cur->sh_name == 0) {
			continue;
		}

		RdString sectName = call(ResolveSectName, cur->sh_name);

		if (String_Equals(sectName, name)) {
			return (RdBuffer) {
				.ptr = this->base + cur->sh_offset,
				.len = cur->sh_size
			};
		}
	}

	return (RdBuffer) {
		.ptr = NULL
	};
}

overload def(RdBuffer, GetSection, ref(Word) type) {
	ref(Ehdr) *header  = this->base;
	ref(Shdr) *section = this->base + header->e_shoff;

	fwd(i, header->e_shnum) {
		ref(Shdr) *cur = &section[i];

		if (cur->sh_type == type) {
			return (RdBuffer) {
				.ptr = this->base + cur->sh_offset,
				.len = cur->sh_size
			};
		}
	}

	return (RdBuffer) {
		.ptr = NULL
	};
}

def(void, Each, RdString name, ref(OnSection) onSection) {
	assert(name.len != 0);

	ref(Ehdr) *header  = this->base;
	ref(Shdr) *section = this->base + header->e_shoff;

	fwd(i, header->e_shnum) {
		ref(Shdr) *cur = &section[i];

		if (cur->sh_name == 0) {
			continue;
		}

		RdString sectName = call(ResolveSectName, cur->sh_name);

		if (String_BeginsWith(sectName, name)) {
			bool cont = callbackRet(onSection, false, sectName, (RdBuffer) {
				.ptr = this->base + cur->sh_offset,
				.len = cur->sh_size
			});

			if (!cont) {
				break;
			}
		}
	}
}

static sdef(short, Compare, ref(Symbol) *left, ref(Symbol) *right) {
	return Integer_Compare(
		(IntPtr) left->address,
		(IntPtr) right->address);
}

/* Reads the static symbol table, SHT_SYMTAB, as opposed to
 * SHT_DYNSYM, the dynamic symbol table.
 */
def(ref(Symbols) *, GetSymbolTable) {
	ref(Ehdr) *header  = this->base;
	ref(Shdr) *section = this->base + header->e_shoff;

	char *strtab  = NULL;
	void *ptrSect = NULL;
	size_t lenSect = 0;

	fwd(i, header->e_shnum) {
		ref(Shdr) *cur = &section[i];

		if (cur->sh_type == SHT_SYMTAB) {
			assert(cur->sh_entsize == sizeof(ref(Sym)));

			/* `sh_link' is an offset pointing to the `.strtab' section. */
			ref(Shdr) *link = &section[cur->sh_link];
			strtab = this->base + link->sh_offset;

			ptrSect = this->base + cur->sh_offset;
			lenSect = cur->sh_size;

			break;
		}
	}

	if (ptrSect == NULL) {
		return scall(Symbols_New, 0);
	}

	size_t num = lenSect / sizeof(ref(Sym));

	ref(Symbols) *symbols = scall(Symbols_New, num);

	for (ref(Sym) *cur = ptrSect; (void *) cur < ptrSect + lenSect; cur++) {
		if (cur->st_shndx == SHN_UNDEF) {
			continue;
		}

		char *name = strtab + cur->st_name;

		if (*name == '\0') {
			continue;
		}

		if (ELF32_ST_TYPE(cur->st_info) == STT_FILE) {
			continue;
		}

		assert((void *) cur->st_value != NULL);

		scall(Symbols_Push, &symbols, (ref(Symbol)) {
			.name    = String_FromNul(name),
			.address = (void *) cur->st_value
		});
	}

	Array_Sort(symbols, ref(Compare));

	return symbols;
}

rsdef(RdString, Symbols_FindNearest, ref(Symbols) *symbols, void *addr) {
	if (symbols->len == 0) {
		return $("");
	}

	size_t low  = 0;
	size_t high = symbols->len - 1;

	if (addr < symbols->buf[low].address ||
		addr > symbols->buf[high].address)
	{
		return $("");
	}

	while (high > low + 1) {
		size_t mid = (high + low) / 2;

		if (addr >= symbols->buf[mid].address) {
			low = mid;
		} else {
			high = mid;
		}
	}

	return symbols->buf[low].name;
}
