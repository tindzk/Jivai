#import <signal.h>

#import "String.h"
#import "Compiler.h"
#import "Exception.h"

#define self Signal

// @exc SigAlrm
// @exc SigBus
// @exc SigFpe
// @exc SigIll
// @exc SigInt
// @exc SigPipe
// @exc SigQuit
// @exc SigSegv
// @exc SigTerm
// @exc SignalHandlerNotSet
// @exc Unknown

/* This structure mirrors the one found in /usr/include/asm/ucontext.h */
record(ref(UserContext)) {
	unsigned long     uc_flags;
	struct ucontext   *uc_link;
	stack_t           uc_stack;
	struct sigcontext uc_mcontext;
	sigset_t          uc_sigmask;
};

void Signal0(void);

sdef(void, Register, int signal, void (*cb)(int, siginfo_t *, void *));
sdef(void, Ignore, int signal);
sdef(void, OnSignal, int signal, __unused siginfo_t *info, __unused void *ucontext);

#undef self
