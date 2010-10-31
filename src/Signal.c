#import "Signal.h"

static ExceptionManager *exc;

void Signal0(ExceptionManager *e) {
	exc = e;

	/* Register these signals as exceptions. */
	Signal_Register(SIGALRM, ref(OnSignal));
	Signal_Register(SIGBUS,  ref(OnSignal));
	Signal_Register(SIGFPE,  ref(OnSignal));
	Signal_Register(SIGILL,  ref(OnSignal));
	Signal_Register(SIGINT,  ref(OnSignal));
	Signal_Register(SIGQUIT, ref(OnSignal));
	Signal_Register(SIGSEGV, ref(OnSignal));
	Signal_Register(SIGTERM, ref(OnSignal));
	Signal_Register(SIGPIPE, ref(OnSignal));
}

sdef(void, Register, int signal, void (*cb)(int, siginfo_t *, void *)) {
	struct sigaction sigact;

	sigemptyset(&sigact.sa_mask);

	sigact.sa_flags     = SA_RESTART | SA_SIGINFO;
	sigact.sa_sigaction = cb;
	sigact.sa_restorer  = NULL;

	if (sigaction(signal, &sigact, (struct sigaction *) NULL) != 0) {
		throw(exc, excSignalHandlerNotSet);
	}
}

sdef(void, Ignore, int signal) {
	struct sigaction sigact;

	sigemptyset(&sigact.sa_mask);

	sigact.sa_flags   = 0;
	sigact.sa_handler = SIG_IGN;

	if (sigaction(signal, &sigact, NULL) != 0) {
		throw(exc, excSignalHandlerNotSet);
	}
}

sdef(void, OnSignal, int signal, __unused siginfo_t *info, __unused void *ucontext) {
	size_t code;

	if (signal == SIGALRM) {
		code = excSigAlrm;
		exc->e.scode = String("excSigAlrm");
	} else if (signal == SIGBUS) {
		code = excSigBus;
		exc->e.scode = String("excSigBus");
	} else if (signal == SIGFPE) {
		code = excSigFpe;
		exc->e.scode = String("excSigFpe");
	} else if (signal == SIGILL) {
		code = excSigIll;
		exc->e.scode = String("excSigIll");
	} else if (signal == SIGINT) {
		code = excSigInt;
		exc->e.scode = String("excSigInt");
	} else if (signal == SIGQUIT) {
		code = excSigQuit;
		exc->e.scode = String("excSigQuit");
	} else if (signal == SIGSEGV) {
		code = excSigSegv;
		exc->e.scode = String("excSigSegv");
	} else if (signal == SIGTERM) {
		code = excSigTerm;
		exc->e.scode = String("excSigTerm");
	} else if (signal == SIGPIPE) {
		code = excSigPipe;
		exc->e.scode = String("excSigPipe");
	} else {
		code = excUnknown;
		exc->e.scode = String("excUnknown");
	}

#if Exception_SaveTrace
	exc->e.traceItems = Backtrace_GetTrace(exc->e.trace, Exception_TraceSize);

	/* Overwrite the first trace item with the address from which the signal was raised. */
	ref(UserContext) *uc = (ref(UserContext) *) ucontext;

	#if defined(__i386__)
		exc->e.trace[0] = (void *) uc->uc_mcontext.eip;
	#elif defined(__x86_64__)
		exc->e.trace[0] = (void *) uc->uc_mcontext.rip;
	#endif
#endif

#if Exception_SaveOrigin
	exc->e.func = String(__func__);
#endif

	ExceptionManager_Raise(exc, Modules_Signal + code);
}
