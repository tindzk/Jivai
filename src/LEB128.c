#import "LEB128.h"

#define self LEB128

rsdef(size_t, ReadSigned, ubyte *addr, s32 *ret) {
	int shift = 0;
	size_t count = 0;

	*ret = 0;

	for (;;) {
		*ret |= (*addr & 0x7f) << shift;
		shift += 7;

		count++;

		if ((*addr & 0x80) == 0) {
			break;
		}

		addr++;
	}

	assert(sizeof(*ret) == 4);

	if (shift < 8 * sizeof(*ret) && (*addr & 0x40) != 0) {
		/* Sign-extend negative value. */
		*ret |= -1LL << shift;
	}

	return count;
}

rsdef(size_t, ReadUnsigned, ubyte *addr, u32 *ret) {
	int shift = 0;
	size_t count = 0;

	*ret = 0;

	for (;;) {
		*ret |= (*addr & 0x7f) << shift;
		shift += 7;

		count++;

		if ((*addr & 0x80) == 0) {
			break;
		}

		addr++;
	}

	return count;
}
