/*
 * This module is based upon Jeff Muizelaar's backtrace-symbols library.
 */

/*
 * A hacky replacement for backtrace_symbols in glibc
 *
 * backtrace_symbols in glibc looks up symbols using dladdr which is limited in
 * the symbols that it sees. libbacktracesymbols opens the executable and shared
 * libraries using libbfd and will look up backtrace information using the symbol
 * table and the dwarf line information.
 *
 * It may make more sense for this program to use libelf instead of libbfd.
 * However, I have not investigated that yet.
 *
 * Derived from addr2line.c from GNU Binutils by Jeff Muizelaar
 *
 * Copyright 2007 Jeff Muizelaar
 */

/* addr2line.c -- convert addresses to line number and function name
 *
 * Copyright 1997, 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
 * Contributed by Ulrich Lauther <Ulrich.Lauther@mchp.siemens.de>
 *
 * This file was part of GNU Binutils.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#import "BFD.h"

/* Symbol table. */
static asymbol **syms;

/* Read in the symbol table. */
static void BFD_ReadSymtab(bfd *abfd) {
	long symcount;
	u32 size;

	if (!BitMask_Has(bfd_get_file_flags(abfd), HAS_SYMS)) {
		return;
	}

	symcount = bfd_read_minisymbols(abfd, false, (void *) &syms, &size);

	if (symcount == 0) {
		free(syms);

		symcount = bfd_read_minisymbols(
			abfd,
			true /* dynamic */,
			(void *) &syms,
			&size
		);
	}

	if (symcount < 0) {
		const char *errmsg = bfd_errmsg(bfd_get_error());
		fprintf(stderr, "%s: %s\n", bfd_get_filename(abfd), errmsg);
		System_Exit(ExitStatus_Failure);
	}
}

void BFD_TranslateAddress(bfd *abfd, bfd_vma pc, BFD_Item *cur) {
	cur->function = String_New(0);
	cur->filename = String_New(0);
	cur->addr     = (long long unsigned int) pc;
	cur->line     = 0;

	const char *filename;
	const char *function;

	for (asection *p = abfd->sections; p != NULL; p = p->next) {
		bfd_vma vma;
		bfd_size_type size;

		if (!BitMask_Has(bfd_get_section_flags(abfd, p), SEC_ALLOC)) {
			continue;
		}

		vma = bfd_get_section_vma(abfd, p);
		if (pc < vma) {
			continue;
		}

		size = bfd_section_size(abfd, p);
		if (pc >= vma + size) {
			continue;
		}

		if (bfd_find_nearest_line(abfd, p, syms, pc - vma, &filename, &function, &cur->line)) {
			if (function != NULL) {
				String_Copy(&cur->function,
					String_FromNul((char *) function));
			}

			if (filename != NULL) {
				String_Copy(&cur->filename,
					String_FromNul((char *) filename));
			}

			return;
		}
	}
}

void BFD_ProcessFile(const char *filename, bfd_vma addr, BFD_Item *cur) {
	bfd *abfd;
	char **matching;

	abfd = bfd_openr(filename, NULL);

	if (abfd == NULL) {
		fprintf(stderr, "%s: bfd_openr() returned NULL", filename);
		System_Exit(ExitStatus_Failure);
	}

	if (bfd_check_format(abfd, bfd_archive)) {
		fprintf(stderr, "%s: can not get addresses from archive", filename);
		System_Exit(ExitStatus_Failure);
	}

	if (!bfd_check_format_matches(abfd, bfd_object, &matching)) {
		const char *errmsg = bfd_errmsg(bfd_get_error());

		fprintf(stderr, "%s: %s\n", bfd_get_filename(abfd), errmsg);

		if (bfd_get_error() == bfd_error_file_ambiguously_recognized) {
			/* After a FALSE return from bfd_check_format_matches with
			 * bfd_get_error () == bfd_error_file_ambiguously_recognized, print
			 * the possible matching targets. */
			
			fprintf(stderr, "Matching formats: ");

			while (*matching) {
				fprintf(stderr, " %s", *matching++);
			}

			fputc('\n', stderr);

			free(matching);
		}

		System_Exit(ExitStatus_Failure);
	}

	BFD_ReadSymtab(abfd);

	BFD_TranslateAddress(abfd, addr, cur);

	if (syms != NULL) {
		free(syms);
		syms = NULL;
	}

	bfd_close(abfd);
}

static int BFD_FindMatchingFile(struct dl_phdr_info *info, __unused size_t size, void *data) {
	BFD_FileMatch *match = data;
	/* This code is modeled from Gfind_proc_info-lsb.c:callback() from libunwind */
	long n;
	const ElfW(Phdr) *phdr;
	ElfW(Addr) load_base = info->dlpi_addr;
	phdr = info->dlpi_phdr;

	for (n = info->dlpi_phnum; --n >= 0; phdr++) {
		if (phdr->p_type == PT_LOAD) {
			ElfW(Addr) vaddr = phdr->p_vaddr + load_base;

			if (((typeof(vaddr)) match->address >= vaddr)
			 && ((typeof(vaddr)) match->address < (vaddr + phdr->p_memsz))) {
				/* we found a match */
				match->file = info->dlpi_name;
				match->base = (void *) info->dlpi_addr;
			}
		}
	}

	return 0;
}

BFD_Item* BFD_ResolveSymbols(void *const *buffer, int size) {
	int stack_depth = size - 1;
	int x, y;

	BFD_Item *res = malloc(100 * sizeof(BFD_Item));
	int cnt = 0;

	bfd_init();

	for (x = stack_depth, y = 0; x >= 0; x--, y++) {
		BFD_FileMatch match = { .address = buffer[x] };
		bfd_vma addr;

		dl_iterate_phdr(BFD_FindMatchingFile, &match);
		addr = buffer[x] - match.base;

		if (match.file && strlen(match.file)) {
			BFD_ProcessFile(match.file, addr, &res[cnt]);
		} else {
			BFD_ProcessFile("/proc/self/exe", addr, &res[cnt]);
		}

		res = realloc(res, (cnt + 100) * sizeof(BFD_Item));
		cnt++;
	}

	return res;
}
