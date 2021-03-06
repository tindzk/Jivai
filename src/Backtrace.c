#import "Backtrace.h"

#define self Backtrace

sdef(void, PrintTrace, void **addr, uint size) {
	ELF elf;

	if (!System_IsRunningOnValgrind()) {
		elf = ELF_New($("/proc/self/exe"));
	} else {
		/* As for Valgrind, we have to follow the symbolic link manually. */
		String link = Path_followLink($("/proc/self/exe"));
		elf = ELF_New(link.rd);
		String_Destroy(&link);
	}

	DWARF dwarf = DWARF_New(ELF_GetSection(&elf, $(".debug_line")));
	DWARF_ParseLineNumberProgram(&dwarf);

	ELF_Symbols *symbols = ELF_GetSymbolTable(&elf);

	System_err($("Traceback (most recent call first):\n"));

	fwd(i, size) {
		System_err($("\tat "));

		RdString function = ELF_Symbols_FindNearest(symbols, addr[i]);

		if (function.len == 0) {
			System_err($("0x"));

			String hex = Hex_ToString(addr[i]);
			System_err(hex.rd);
			String_Destroy(&hex);
		} else {
			System_err(function);
			System_err($("("));

			assert(sizeof(void *) == sizeof(DWARF_Pointer));
			DWARF_Match match = DWARF_ResolveSymbol(&dwarf, (DWARF_Pointer) addr[i]);

			if (match.file.name.len > 0) {
				String line = Integer_ToString(match.line);

				System_err(match.file.name);
				System_err($(":"));
				System_err(line.rd);

				String_Destroy(&line);
			}

			System_err($(")"));
		}

		System_err($("\n"));
	}

	ELF_Symbols_Free(symbols);
	DWARF_Destroy(&dwarf);
	ELF_Destroy(&elf);
}

inline sdef(void *, GetFrameAddr, uint level) {
	switch (level) {
		case 1: return __builtin_frame_address(1);
		case 2: return __builtin_frame_address(2);
		case 3: return __builtin_frame_address(3);
		case 4: return __builtin_frame_address(4);
		case 5: return __builtin_frame_address(5);
		case 6: return __builtin_frame_address(6);
		case 7: return __builtin_frame_address(7);
		case 8: return __builtin_frame_address(8);
		case 9: return __builtin_frame_address(9);
		case 10: return __builtin_frame_address(10);
		case 11: return __builtin_frame_address(11);
		case 12: return __builtin_frame_address(12);
		case 13: return __builtin_frame_address(13);
		case 14: return __builtin_frame_address(14);
		case 15: return __builtin_frame_address(15);
		case 16: return __builtin_frame_address(16);
		default: return NULL;
	}
}

inline sdef(void *, GetReturnAddr, uint level) {
	switch (level) {
		case 1: return __builtin_return_address(1);
		case 2: return __builtin_return_address(2);
		case 3: return __builtin_return_address(3);
		case 4: return __builtin_return_address(4);
		case 5: return __builtin_return_address(5);
		case 6: return __builtin_return_address(6);
		case 7: return __builtin_return_address(7);
		case 8: return __builtin_return_address(8);
		case 9: return __builtin_return_address(9);
		case 10: return __builtin_return_address(10);
		case 11: return __builtin_return_address(11);
		case 12: return __builtin_return_address(12);
		case 13: return __builtin_return_address(13);
		case 14: return __builtin_return_address(14);
		case 15: return __builtin_return_address(15);
		case 16: return __builtin_return_address(16);
		default: return NULL;
	}
}

sdef(uint, GetTrace, void **buf, uint size) {
	uint i;

	for (i = 1; i < size; i++) {
		if (Backtrace_GetFrameAddr(i) == NULL) {
			break;
		}

		if ((buf[i - 1] = Backtrace_GetReturnAddr(i)) == NULL) {
			break;
		}
	}

	return i - 1;
}
