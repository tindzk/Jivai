#import "UniStd.h" /* __environ */
#import "String.h"
#import "Kernel.h"
#import "Channel.h"
#import "Exception.h"

#define self System

static inline sdef(void, exit, ExitStatus status) {
	Kernel_exit(status);
}

static inline sdef(void, out, RdString s) {
	Channel_Write(Channel_StdOut, s);
}

static inline sdef(void, err, RdString s) {
	Channel_Write(Channel_StdErr, s);
}

sdef(bool, IsRunningOnValgrind);
sdef(RdString, getLanguage);
sdef(bool, IsDebugging);

#undef self
