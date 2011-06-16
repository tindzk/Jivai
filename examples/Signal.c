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
	}

	return true;
}
