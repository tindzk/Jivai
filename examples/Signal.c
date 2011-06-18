#import <Main.h>

#define self Application

def(bool, Run) {
	if (this->args->len == 0 || this->args->buf[0].len == 0) {
		return false;
	}

	if (this->args->buf[0].buf[0] == 'a') {
		/* Insufficient permissions (stored in rodata segment). */
		char *ptr = (char *) "test";
		ptr[1] = 'c';
	} else if (this->args->buf[0].buf[0] == 'b') {
		/* Not mapped. */
		int *ptr = NULL;
		*ptr = 0;
	} else if (this->args->buf[0].buf[0] == 'c') {
		/* Illegal operand (see http://en.wikipedia.org/wiki/SIGILL). */
		const static unsigned char insn[4] = {
			0xff, 0xff, 0xff, 0xff
		};

		((void (*)()) insn)();
	} else if (this->args->buf[0].buf[0] == 'd') {
		/* Integer division by zero.
		 * `volatile' needed to eliminate compile-time optimizations
		 * (see http://en.wikipedia.org/wiki/SIGFPE).
		 */
		volatile int x = 42;
		volatile int y = 0;
		x = x / y;
	} else if (this->args->buf[0].buf[0] == 'e') {
		/* Floating point division by zero. */
		volatile float x = 45;
		volatile float y = 0;
		x = x / y;
	}

	return true;
}
