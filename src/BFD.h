#ifndef BFD_H
#define BFD_H

#include <bfd.h>

#define _GNU_SOURCE
#include <libiberty.h>
#undef _GNU_SOURCE

#include <link.h>

#include "Memory.h"
#include "Compiler.h"

typedef struct {
	String filename;
	String function;
	size_t line;
	long long unsigned int addr;
} BFD_Item;

typedef struct {
	const char *file;
	void *address;
	void *base;
	void *hdr;
} BFD_FileMatch;

BFD_Item* BFD_ResolveSymbols(void *const *buffer, int size);

#endif
