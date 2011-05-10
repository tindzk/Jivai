#import "UniStd.h" /* __environ */
#import "String.h"
#import "Kernel.h"
#import "Channel.h"
#import "Exception.h"

#define self System

static inline sdef(void, Exit, ExitStatus status) {
	Kernel_exit(status);
}

static inline sdef(void, Out, RdString s) {
	Channel_Write(Channel_StdOut, s);
}

static inline sdef(void, Err, RdString s) {
	Channel_Write(Channel_StdErr, s);
}

#define String_Print(s) System_Out(s)

sdef(bool, IsRunningOnValgrind);
sdef(bool, IsDebugging);

#undef self
