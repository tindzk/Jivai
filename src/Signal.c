#import "Signal.h"

static ExceptionManager *exc;

Exception_Define(Signal_SigAlrmException);
Exception_Define(Signal_SigBusException);
Exception_Define(Signal_SigFpeException);
Exception_Define(Signal_SigIllException);
Exception_Define(Signal_SigIntException);
Exception_Define(Signal_SigPipeException);
Exception_Define(Signal_SigQuitException);
Exception_Define(Signal_SigSegvException);
Exception_Define(Signal_SigTermException);
Exception_Define(Signal_SignalHandlerNotSetException);
Exception_Define(Signal_UnknownException);

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
		throw(exc, &Signal_SignalHandlerNotSetException);
	}
}

void Signal_Ignore(int signal) {
	struct sigaction sigact;

	sigemptyset(&sigact.sa_mask);

	sigact.sa_flags   = 0;
	sigact.sa_handler = SIG_IGN;

	if (sigaction(signal, &sigact, NULL) != 0) {
		throw(exc, &Signal_SignalHandlerNotSetException);
	}
}

void Signal_OnSignal(int signal, UNUSED siginfo_t *info, UNUSED void *ucontext) {
	if (signal == SIGALRM) {
		exc->e.p = &Signal_SigAlrmException;
	} else if (signal == SIGBUS) {
		exc->e.p = &Signal_SigBusException;
	} else if (signal == SIGFPE) {
		exc->e.p = &Signal_SigFpeException;
	} else if (signal == SIGILL) {
		exc->e.p = &Signal_SigIllException;
	} else if (signal == SIGINT) {
		exc->e.p = &Signal_SigIntException;
	} else if (signal == SIGQUIT) {
		exc->e.p = &Signal_SigQuitException;
	} else if (signal == SIGSEGV) {
		exc->e.p = &Signal_SigSegvException;
	} else if (signal == SIGTERM) {
		exc->e.p = &Signal_SigTermException;
	} else if (signal == SIGPIPE) {
		exc->e.p = &Signal_SigPipeException;
	} else {
		exc->e.p = &Signal_UnknownException;
	}

#ifdef Exception_SaveTrace
	exc->e.traceItems = Backtrace_GetTrace(exc->e.trace, Exception_TraceSize);

	/* Overwrite the first trace item with the address from which the signal was raised. */
	sig_ucontext_t *uc = (sig_ucontext_t *) ucontext;

#if defined(__i386__)
	exc->e.trace[0] = (void *) uc->uc_mcontext.eip;
#elif defined(__x86_64__)
	exc->e.trace[0] = (void *) uc->uc_mcontext.rip;
#endif
#endif

#ifdef Exception_SaveOrigin
	exc->e.file = String(__FILE__);
	exc->e.line = __LINE__;
#endif

	ExceptionManager_Raise(exc);
}
