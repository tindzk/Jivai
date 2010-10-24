#import "Runtime.h"

void noReturn Runtime_Exit(ExitStatus status) {
	Kernel_exit(status);
}
