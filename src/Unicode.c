#include "Unicode.h"

/* Taken from BusyBox (busybox-1.16.1/libbb/unicode.c). */
size_t Unicode_CalcWidth(const char *src) {
	size_t bytes = 0;
	unsigned c = (unsigned char) *src;

	if (c <= 0x7f) {
		return 0;
	}

	/* 80-7FF -> 110yyyxx 10xxxxxx */
	/* 800-FFFF -> 1110yyyy 10yyyyxx 10xxxxxx */
	/* 10000-1FFFFF -> 11110zzz 10zzyyyy 10yyyyxx 10xxxxxx */
	/* 200000-3FFFFFF -> 111110tt 10zzzzzz 10zzyyyy 10yyyyxx 10xxxxxx */
	/* 4000000-FFFFFFFF -> 111111tt 10tttttt 10zzzzzz 10zzyyyy 10yyyyxx 10xxxxxx */

	do {
		c <<= 1;
		bytes++;
	} while ((c & 0x80) && bytes < 6);

	return bytes;
}

size_t Unicode_Next(String s, size_t offset) {
	if (offset >= s.len) {
		return 0;
	}

	size_t width = Unicode_CalcWidth(s.buf + offset);

	if (width == 0) {
		return 1;
	} else if (width > 1) {
		return width;
	}

	return 0;
}

size_t Unicode_Prev(String s, size_t offset) {
	size_t tries = 0;
	size_t width = 0;

	while (true) {
		width = Unicode_Next(s, offset - 1);

		if (width > 0) {
			break;
		}

		tries++;

		if (offset == 0 || tries == 6) {
			break;
		}

		offset--;
	}

	return width;
}

size_t Unicode_Count(String s) {
	size_t cnt = 0;
	size_t offset = 0;

	while (offset < s.len) {
		size_t width = Unicode_Next(s, offset);

		if (width == 0) {
			break;
		}

		offset += width;
		cnt++;
	}

	return cnt;
}
