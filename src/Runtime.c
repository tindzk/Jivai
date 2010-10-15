#import "Runtime.h"

void Runtime_Exit(ExitStatus status) {
	Kernel_exit(status);
}
