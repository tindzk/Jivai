#import <bfd.h>

#define _GNU_SOURCE
#include <libiberty.h>
#undef _GNU_SOURCE

#import <link.h>

#import "Memory.h"
#import "BitMask.h"
#import "Runtime.h"
#import "Compiler.h"

typedef struct {
	String filename;
	String function;
	u32 line;
	long long unsigned int addr;
} BFD_Item;

typedef struct {
	const char *file;
	void *address;
	void *base;
	void *hdr;
} BFD_FileMatch;

BFD_Item* BFD_ResolveSymbols(void *const *buffer, int size);
