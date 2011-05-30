#import "LEB128.h"

#define self LEB128

/* Decodes signed LEB128 data. The algorithm is taken from Appendix
 * C of the DWARF 3 spec. Returns the number of bytes read.
 */

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

	/* The number of bits in a signed integer. */
	int numBits = 8 * sizeof(*ret);

	if (shift < numBits && (*addr & 0x40) != 0) {
		/* Sign-extend negative value. */
		*ret |= -1LL << shift;
	}

	return count;
}

/* Decodes an unsigned LEB128 encoded datum. The algorithm is taken
 * from Appendix C of the DWARF 3 spec. For information on the
 * encodings refer to section "7.6 - Variable Length Data". Returns
 * the number of bytes read.
 */

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
