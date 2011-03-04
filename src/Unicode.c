#import "Unicode.h"

#define self Unicode

/* Taken from BusyBox (busybox-1.16.1/libbb/unicode.c). */
sdef(size_t, CalcWidth, const char *src) {
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
	} while (Charset_IsHighBitSet(c) && bytes < 6);

	return bytes;
}

sdef(size_t, Next, ProtString s, size_t offset) {
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

sdef(size_t, Prev, ProtString s, size_t offset) {
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

overload sdef(size_t, Count, ProtString s, size_t offset, size_t len) {
	size_t i   = offset;
	size_t cnt = 0;

	while (i < offset + len) {
		size_t width = Unicode_Next(s, i);

		if (width == 0) {
			break;
		}

		i += width;
		cnt++;
	}

	return cnt;
}

overload sdef(size_t, Count, ProtString s) {
	return Unicode_Count(s, 0, s.len);
}

sdef(void, Shrink, String *s, size_t len) {
	size_t cnt    = 0;
	size_t offset = 0;

	while (offset < s->len) {
		if (cnt == len) {
			String_Crop(s, 0, offset);
			break;
		}

		size_t width = Unicode_Next(s->prot, offset);

		if (width == 0) {
			break;
		}

		offset += width;
		cnt++;
	}
}

sdef(void, ToMultiByte, int c, String *res) {
    if (c <= 0x7F) {
		res->buf[0] = c;
		res->len = 1;
    } else if (c <= 0x7FF) {
        res->buf[0] = 0xC0 | c >> 6;
		res->buf[1] = 0x80 | c & 0x3F;
		res->len = 2;
    } else if (c <= 0xFFFF) {
        res->buf[0] = 0xE0 | c >> 12;
		res->buf[1] = 0x80 | c >> 6 & 0x3F;
		res->buf[2] = 0x80 | c      & 0x3F;
		res->len = 3;
    } else if (c <= 0x10FFFF) {
        res->buf[0] = 0xF0 | c >> 18;
		res->buf[1] = 0x80 | c >> 12 & 0x3F;
		res->buf[2] = 0x80 | c >>  6 & 0x3F;
        res->buf[3] = 0x80 | c       & 0x3F;
		res->len = 4;
    } else {
		res->len = 0;
	}
}
