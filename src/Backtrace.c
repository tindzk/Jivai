#import "Backtrace.h"

void Backtrace_PrintTrace(__unused void **dest, __unused size_t size) {
#ifdef Backtrace_HasBFD
	BFD_Item *items = BFD_ResolveSymbols((void *const *) dest, size);

	String_Print(String("Traceback (most recent call first):\n"));

	for (ssize_t i = size - 1; i >= 0; i--) {
		String_Print(String("\tat "));

		if (items[i].function.len == 0) {
			String_Print(String("0x"));

			String hex = Hex_ToString(items[i].addr);
			String_Print(hex);
			String_Destroy(&hex);
		} else {
			String_Print(items[i].function);
			String_Print(String("("));

			if (items[i].filename.len > 0) {
				ssize_t pos = String_ReverseFind(items[i].filename, '/');

				if (pos != -1) {
					String_Crop(&items[i].filename, pos + 1);
				}

				String_Print(items[i].filename);
				String_Print(String(":"));
				String_Print(Integer_ToString(items[i].line));
			}

			String_Print(String(")"));
		}

		String_Destroy(&items[i].filename);
		String_Destroy(&items[i].function);

		String_Print(String("\n"));
	}

	Memory_Free(items);
#endif
}

void* Backtrace_GetFrameAddr(int level) {
	switch (level + 1) {
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

void* Backtrace_GetReturnAddr(int level) {
	switch (level + 1) {
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

size_t Backtrace_GetTrace(void **buf, size_t size) {
	size_t i;

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
