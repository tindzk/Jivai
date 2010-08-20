#import <signal.h>

#import "String.h"
#import "Compiler.h"
#import "Exception.h"

#undef self
#define self Signal

Exception_Export(SigAlrmException);
Exception_Export(SigBusException);
Exception_Export(SigFpeException);
Exception_Export(SigIllException);
Exception_Export(SigIntException);
Exception_Export(SigPipeException);
Exception_Export(SigQuitException);
Exception_Export(SigSegvException);
Exception_Export(SigTermException);
Exception_Export(SignalHandlerNotSetException);
Exception_Export(UnknownException);

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
