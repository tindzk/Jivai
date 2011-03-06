#import "Signal.h"

#define self Signal

void Signal0(void) {
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
		throw(SignalHandlerNotSet);
	}
}

sdef(void, Ignore, int signal) {
	struct sigaction sigact;

	sigemptyset(&sigact.sa_mask);

	sigact.sa_flags   = 0;
	sigact.sa_handler = SIG_IGN;

	if (sigaction(signal, &sigact, NULL) != 0) {
		throw(SignalHandlerNotSet);
	}
}

sdef(void, OnSignal, int signal, __unused siginfo_t *info, __unused void *ucontext) {
	int code;

	if (signal == SIGALRM) {
		code = ref(SigAlrm);
		__exc_mgr.details.scode = $("excSigAlrm");
	} else if (signal == SIGBUS) {
		code = ref(SigBus);
		__exc_mgr.details.scode = $("excSigBus");
	} else if (signal == SIGFPE) {
		code = ref(SigFpe);
		__exc_mgr.details.scode = $("excSigFpe");
	} else if (signal == SIGILL) {
		code = ref(SigIll);
		__exc_mgr.details.scode = $("excSigIll");
	} else if (signal == SIGINT) {
		code = ref(SigInt);
		__exc_mgr.details.scode = $("excSigInt");
	} else if (signal == SIGQUIT) {
		code = ref(SigQuit);
		__exc_mgr.details.scode = $("excSigQuit");
	} else if (signal == SIGSEGV) {
		code = ref(SigSegv);
		__exc_mgr.details.scode = $("excSigSegv");
	} else if (signal == SIGTERM) {
		code = ref(SigTerm);
		__exc_mgr.details.scode = $("excSigTerm");
	} else if (signal == SIGPIPE) {
		code = ref(SigPipe);
		__exc_mgr.details.scode = $("excSigPipe");
	} else {
		code = ref(Unknown);
		__exc_mgr.details.scode = $("excUnknown");
	}

#if Exception_SaveTrace
	__exc_mgr.details.trace.len = Backtrace_GetTrace(
		__exc_mgr.details.trace.buf,
		Exception_TraceSize);

	/* Overwrite the first trace item with the address from which the signal was raised. */
	ref(UserContext) *uc = (ref(UserContext) *) ucontext;

	#if defined(__i386__)
		__exc_mgr.details.trace.buf[0] = (void *) uc->uc_mcontext.eip;
	#elif defined(__x86_64__)
		__exc_mgr.details.trace.buf[0] = (void *) uc->uc_mcontext.rip;
	#endif
#endif

#if Exception_SaveOrigin
	__exc_mgr.details.func = $(__func__);
#endif

	Exception_Raise(code);
}
