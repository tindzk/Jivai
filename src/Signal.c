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
	} else if (signal == SIGBUS) {
		code = ref(SigBus);
	} else if (signal == SIGFPE) {
		code = ref(SigFpe);
	} else if (signal == SIGILL) {
		code = ref(SigIll);
	} else if (signal == SIGINT) {
		code = ref(SigInt);
	} else if (signal == SIGQUIT) {
		code = ref(SigQuit);
	} else if (signal == SIGSEGV) {
		code = ref(SigSegv);
	} else if (signal == SIGTERM) {
		code = ref(SigTerm);
	} else if (signal == SIGPIPE) {
		code = ref(SigPipe);
	} else {
		code = ref(Unknown);
	}

#if Exception_SaveTrace
	Exception_SetTrace();

	/* Overwrite the first trace item with the address from which the signal was raised. */
	ref(UserContext) *uc = (ref(UserContext) *) ucontext;

	#if defined(__i386__)
		__exc_mgr.details.trace.buf[0] = (void *) uc->uc_mcontext.eip;
	#elif defined(__x86_64__)
		__exc_mgr.details.trace.buf[0] = (void *) uc->uc_mcontext.rip;
	#endif
#endif

#if Exception_SaveOrigin
	Exception_SetOrigin($(__func__));
#endif

	Exception_Raise(code);
}
