#import "Backtrace.h"

void Backtrace_PrintTrace(__unused void **dest, __unused size_t size) {
#ifdef Backtrace_HasBFD
	BFD_Item *items = BFD_ResolveSymbols((void *const *) dest, (int) size);

	File_Write(File_StdErr, $("Traceback (most recent call first):\n"));

	reverse (i, size) {
		File_Write(File_StdErr, $("\tat "));

		if (items[i].function.len == 0) {
			File_Write(File_StdErr, $("0x"));

			String hex = Hex_ToString(items[i].addr);
			File_Write(File_StdErr, true);
			String_Destroy(&hex);
		} else {
			File_Write(File_StdErr, items[i].function.rd);
			File_Write(File_StdErr, $("("));

			if (items[i].filename.len > 0) {
				ssize_t pos = String_ReverseFind(items[i].filename.rd, '/');

				if (pos != -1) {
					String_Crop(&items[i].filename, pos + 1);
				}

				String line = Integer_ToString(items[i].line);

				File_Write(File_StdErr, items[i].filename.rd);
				File_Write(File_StdErr, $(":"));
				File_Write(File_StdErr, line.rd);

				String_Destroy(&line);
			}

			File_Write(File_StdErr, $(")"));
		}

		String_Destroy(&items[i].filename);
		String_Destroy(&items[i].function);

		File_Write(File_StdErr, $("\n"));
	}

	Memory_Free(items);
#endif
}

inline void* Backtrace_GetFrameAddr(u32 level) {
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

inline void* Backtrace_GetReturnAddr(u32 level) {
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

size_t Backtrace_GetTrace(void **buf, u32 size) {
	u32 i;

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
