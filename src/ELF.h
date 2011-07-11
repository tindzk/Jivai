#import <elf.h>

#import "String.h"
#import "Buffer.h"
#import "Exception.h"
#import "MemoryMappedFile.h"

#define self ELF

exc(InvalidFile)
exc(UnknownError)

Callback(ref(OnSection), bool, RdString name, RdBuffer sect);

#if defined(__x86_64__)
#define ELF_Class ELFCLASS64
#define ELF_Ehdr  Elf64_Ehdr
#define ELF_Shdr  Elf64_Shdr
#define ELF_Phdr  Elf64_Phdr
#define ELF_Dyn   Elf64_Dyn
#define ELF_Sym   Elf64_Sym
#define ELF_Word  Elf64_Word
#else
#define ELF_Class ELFCLASS32
#define ELF_Ehdr  Elf32_Ehdr
#define ELF_Shdr  Elf32_Shdr
#define ELF_Phdr  Elf32_Phdr
#define ELF_Dyn   Elf32_Dyn
#define ELF_Sym   Elf32_Sym
#define ELF_Word  Elf32_Word
#endif

record(ref(Symbol)) {
	RdString name;
	void *address;
};

Array(ref(Symbol), ref(Symbols));

class {
	void *base;
	MemoryMappedFile file;
};

rsdef(self, New, RdString path);
def(void, Destroy);
def(void *, getRealAddress, void *virtAddr);
overload def(RdBuffer, GetSection, RdString name);
overload def(RdBuffer, GetSection, ref(Word) type);
def(void, Each, RdString name, ref(OnSection) onSection);
def(ref(Symbols) *, GetSymbolTable);
rsdef(RdString, Symbols_FindNearest, ref(Symbols) *symbols, void *addr);

#undef self
