#import <signal.h>

#import "String.h"
#import "Compiler.h"
#import "Exception.h"

#undef self
#define self Signal

enum {
	excSigAlrm = excOffset,
	excSigBus,
	excSigFpe,
	excSigIll,
	excSigInt,
	excSigPipe,
	excSigQuit,
	excSigSegv,
	excSigTerm,
	excSignalHandlerNotSet,
	excUnknown
};

/* This structure mirrors the one found in /usr/include/asm/ucontext.h */
typedef struct _sig_ucontext {
	unsigned long     uc_flags;
	struct ucontext   *uc_link;
	stack_t           uc_stack;
	struct sigcontext uc_mcontext;
	sigset_t          uc_sigmask;
} sig_ucontext_t;

void Signal0(ExceptionManager *e);

void Signal_Register(int signal, void (*cb)(int, siginfo_t *, void *));
void Signal_Ignore(int signal);
void Signal_OnSignal(int signal, siginfo_t *info, void *ucontext);
