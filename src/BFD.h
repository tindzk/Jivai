#import <bfd.h>

#define _GNU_SOURCE
#import <stdlib.h> /* malloc() */
#include <libiberty.h>
#undef _GNU_SOURCE

#import <link.h>

#import "Memory.h"
#import "String.h"
#import "System.h"
#import "BitMask.h"
#import "Compiler.h"

#define self BFD

record(ref(Item)) {
	String filename;
	String function;
	u32 line;
	long long unsigned int addr;
};

record(ref(FileMatch)) {
	const char *file;
	void *address;
	void *base;
	void *hdr;
};

BFD_Item* BFD_ResolveSymbols(void *const *buffer, int size);

#undef self
