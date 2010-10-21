#import "Signal.h"

static ExceptionManager *exc;

void Signal0(ExceptionManager *e) {
	exc = e;

	/* Register these signals as exceptions. */
	Signal_Register(SIGALRM, Signal_OnSignal);
	Signal_Register(SIGBUS,  Signal_OnSignal);
	Signal_Register(SIGFPE,  Signal_OnSignal);
	Signal_Register(SIGILL,  Signal_OnSignal);
	Signal_Register(SIGINT,  Signal_OnSignal);
	Signal_Register(SIGQUIT, Signal_OnSignal);
	Signal_Register(SIGSEGV, Signal_OnSignal);
	Signal_Register(SIGTERM, Signal_OnSignal);
	Signal_Register(SIGPIPE, Signal_OnSignal);
}

void Signal_Register(int signal, void (*cb)(int, siginfo_t *, void *)) {
	struct sigaction sigact;

	sigemptyset(&sigact.sa_mask);

	sigact.sa_flags     = SA_RESTART | SA_SIGINFO;
	sigact.sa_sigaction = cb;
	sigact.sa_restorer  = NULL;

	if (sigaction(signal, &sigact, (struct sigaction *) NULL) != 0) {
		throw(exc, excSignalHandlerNotSet);
	}
}

void Signal_Ignore(int signal) {
	struct sigaction sigact;

	sigemptyset(&sigact.sa_mask);

	sigact.sa_flags   = 0;
	sigact.sa_handler = SIG_IGN;

	if (sigaction(signal, &sigact, NULL) != 0) {
		throw(exc, excSignalHandlerNotSet);
	}
}

void Signal_OnSignal(int signal, __unused siginfo_t *info, __unused void *ucontext) {
	exc->e.module = Modules_Signal;

	if (signal == SIGALRM) {
		exc->e.code  = excSigAlrm;
		exc->e.scode = String("excSigAlrm");
	} else if (signal == SIGBUS) {
		exc->e.code  = excSigBus;
		exc->e.scode = String("excSigBus");
	} else if (signal == SIGFPE) {
		exc->e.code  = excSigFpe;
		exc->e.scode = String("excSigFpe");
	} else if (signal == SIGILL) {
		exc->e.code  = excSigIll;
		exc->e.scode = String("excSigIll");
	} else if (signal == SIGINT) {
		exc->e.code  = excSigInt;
		exc->e.scode = String("excSigInt");
	} else if (signal == SIGQUIT) {
		exc->e.code  = excSigQuit;
		exc->e.scode = String("excSigQuit");
	} else if (signal == SIGSEGV) {
		exc->e.code  = excSigSegv;
		exc->e.scode = String("excSigSegv");
	} else if (signal == SIGTERM) {
		exc->e.code  = excSigTerm;
		exc->e.scode = String("excSigTerm");
	} else if (signal == SIGPIPE) {
		exc->e.code  = excSigPipe;
		exc->e.scode = String("excSigPipe");
	} else {
		exc->e.code  = excUnknown;
		exc->e.scode = String("excUnknown");
	}

#if Exception_SaveTrace
	exc->e.traceItems = Backtrace_GetTrace(exc->e.trace, Exception_TraceSize);

	/* Overwrite the first trace item with the address from which the signal was raised. */
	sig_ucontext_t *uc = (sig_ucontext_t *) ucontext;

	#if defined(__i386__)
		exc->e.trace[0] = (void *) uc->uc_mcontext.eip;
	#elif defined(__x86_64__)
		exc->e.trace[0] = (void *) uc->uc_mcontext.rip;
	#endif
#endif

#if Exception_SaveOrigin
	exc->e.func = String(__func__);
#endif

	ExceptionManager_Raise(exc);
}
