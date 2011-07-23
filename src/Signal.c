#import "Signal.h"

/* Make sure you only instantiate Signal once. Preferrably, use the
 * GetInstance() method. However, it is optional to use Signal.
 */

#define self Signal

Singleton(self);
SingletonDestructor(self);

sdef(void, ignoreSignal, int signal) {
	struct sigaction sigact = {
		.sa_flags   = 0,
		.sa_handler = SIG_IGN
	};

	sigemptyset(&sigact.sa_mask);

	if (sigaction(signal, &sigact, NULL) == -1) {
		throw(UnknownError);
	}
}

rsdef(self, New) {
	self res = {
		/* Initializes callbacks, too. */
		.terminations = NULL
	};

	sigemptyset(&res.mask);

	sigaddset(&res.mask, ref(Type_Terminate));
	sigaddset(&res.mask, ref(Type_Interrupt));
	sigaddset(&res.mask, ref(Type_Quit));
	sigaddset(&res.mask, ref(Type_HangUp));

	sigaddset(&res.mask, ref(Type_ChildStatus));

	sigprocmask(SIG_BLOCK, &res.mask, NULL);

	res.ch = Channel_New(
		signalfd(-1, &res.mask, SFD_NONBLOCK),
		FileStatus_NonBlock);

	if (res.ch.id == -1) {
		throw(UnknownError);
	}

	res.terminations = scall(ChildTerminations_New, 0);

	return res;
}

def(void, Destroy) {
	sigprocmask(SIG_UNBLOCK, &this->mask, NULL);

	scall(ChildTerminations_Free, this->terminations);

	if (this->evLoop != NULL) {
		EventLoop_DetachChannel(
			EventLoop_GetInstance(), this->evLoop, false);
	}

	Channel_Destroy(&this->ch);
}

static def(void, onSignal) {
	struct signalfd_siginfo sig;
	size_t len = Channel_Read(&this->ch, &sig, sizeof(sig));

	if (len != sizeof(sig)) {
		throw(UnknownError);
	}

	if (sig.ssi_signo == ref(Type_ChildStatus)) {
		fwd(i, this->terminations->len) {
			ref(ChildTermination) termination =
				this->terminations->buf[i];

			if (termination.pid == sig.ssi_pid) {
				callback(termination.cb, sig.ssi_pid, sig.ssi_status);
				scall(ChildTerminations_Delete, this->terminations, i);
				break;
			}
		}
	} else if (scall(isExitSignal, sig.ssi_signo)) {
		callback(this->onTerminate, sig.ssi_signo);
	} else {
		callback(this->onCustom, sig.ssi_signo);
	}
}

def(void, listen) {
	/* This function cannot be called twice. */
	assert(this->evLoop == NULL);

	this->evLoop =
		EventLoop_AddChannel(EventLoop_GetInstance(), &this->ch,
			EventLoop_OnInput_For(this, ref(onSignal)),
			EventLoop_OnOutput_Empty(),
			EventLoop_OnDestroy_Empty());
}

static def(void, commit) {
	int newId = signalfd(this->ch.id, &this->mask, SFD_NONBLOCK);

	/* This must be true, otherwise we'd have to update the fd in the
	 * event loop, too.
	 */
	assert(newId == this->ch.id);
}

def(void, add, int signal) {
	assert(scall(isCustomSignal, signal));

	__unused sigset_t old = this->mask;

	int st = sigaddset(&this->mask, signal);
	assert(st != -1);

	/* Already set? */
	assert(!Memory_Equals(&old, &this->mask, sizeof(this->mask)));

	sigprocmask(SIG_SETMASK, &this->mask, NULL);

	call(commit);
}

def(void, delete, int signal) {
	assert(scall(isCustomSignal, signal));

	__unused sigset_t old = this->mask;

	int st = sigdelset(&this->mask, signal);
	assert(st != -1);

	/* Not set. */
	assert(!Memory_Equals(&old, &this->mask, sizeof(this->mask)));

	sigprocmask(SIG_SETMASK, &this->mask, NULL);

	call(commit);
}

def(void, uponCustom, ref(OnCustom) cb) {
	this->onCustom = cb;
}

def(void, uponTermination, ref(OnTerminate) cb) {
	this->onTerminate = cb;
}

def(void, uponChildTermination, pid_t pid, ref(OnChildTerminate) cb) {
	scall(ChildTerminations_Push, &this->terminations, (ref(ChildTermination)) {
		.pid = pid,
		.cb  = cb
	});
}

static noReturn sdef(void, onSystemSignal, int signal, __unused siginfo_t *info, __unused void *ucontext) {
	assert(scall(isSystemSignal, signal));

	int code =
		(signal == ref(Type_BusError))            ? ref(BusError)            :
		(signal == ref(Type_ArithmeticError))     ? ref(ArithmeticError)     :
		(signal == ref(Type_IllegalInstruction))  ? ref(IllegalInstruction)  :
		(signal == ref(Type_InvalidMemoryAccess)) ? ref(InvalidMemoryAccess) :
		(signal == ref(Type_Alarm))               ? ref(Alarm)               :
		(signal == ref(Type_Pipe))                ? ref(Pipe)                : -1;

	if (code == ref(InvalidMemoryAccess)) {
		if (info->si_code == SEGV_ACCERR) {
			printf("Insufficient permissions for %p.\n", info->si_addr);
		} else {
			printf("Address %p not mapped.\n", info->si_addr);
		}
	} else if (code == ref(IllegalInstruction)) {
		if (info->si_code == ILL_ILLOPC) {
			printf("Illegal opcode at %p.\n", info->si_addr);
		} else if (info->si_code == ILL_ILLOPN) {
			printf("Illegal operand at %p.\n", info->si_addr);
		} else if (info->si_code == ILL_ILLADR) {
			printf("Illegal addressing mode at %p.\n", info->si_addr);
		} else if (info->si_code == ILL_ILLTRP) {
			printf("Illegal trap at %p.\n", info->si_addr);
		} else if (info->si_code == ILL_PRVOPC) {
			printf("Privileged opcode at %p.\n", info->si_addr);
		} else if (info->si_code == ILL_PRVREG) {
			printf("Privileged register at %p.\n", info->si_addr);
		} else if (info->si_code == ILL_COPROC) {
			printf("Co-processor error at %p.\n", info->si_addr);
		} else if (info->si_code == ILL_BADSTK) {
			printf("Internal stack error at %p.\n", info->si_addr);
		}
	} else if (code == ref(ArithmeticError)) {
		if (info->si_code == FPE_INTDIV) {
			printf("Integer division by zero %p.\n", info->si_addr);
		} else if (info->si_code == FPE_INTOVF) {
			printf("Integer overflow at %p.\n", info->si_addr);
		} else if (info->si_code == FPE_FLTDIV) {
			printf("Floating point division by zero at %p.\n", info->si_addr);
		} else if (info->si_code == FPE_FLTOVF) {
			printf("Floating point overflow at %p.\n", info->si_addr);
		} else if (info->si_code == FPE_FLTUND) {
			printf("Floating point underflow at %p.\n", info->si_addr);
		} else if (info->si_code == FPE_FLTRES) {
			printf("Floating point inexact result at %p.\n", info->si_addr);
		} else if (info->si_code == FPE_FLTINV) {
			printf("Invalid floating point operation at %p.\n", info->si_addr);
		} else if (info->si_code == FPE_FLTSUB) {
			printf("Subscript out of range at %p.\n", info->si_addr);
		}
	}

#if Exception_SaveTrace
	Exception_SetTrace();

	/* Overwrite the first trace item with the address to which the
	 * signal refers to.
	 */
	ref(UserContext) *uc = (ref(UserContext) *) ucontext;

	#if defined(__i386__)
		__exc_mgr.details.trace.buf[0] = (void *) uc->uc_mcontext.eip;
	#elif defined(__x86_64__)
		__exc_mgr.details.trace.buf[0] = (void *) uc->uc_mcontext.rip;
	#endif
#endif

#if Exception_SaveOrigin
	Exception_SetOrigin($$(__func__));
#endif

	Exception_Raise(code);
}

static sdef(void, registerSystemSignal, int signal) {
	assert(scall(isSystemSignal, signal));

	struct sigaction sigact = {
		.sa_flags     = SA_RESTART | SA_SIGINFO,
		.sa_sigaction = ref(onSystemSignal),
		.sa_restorer  = NULL
	};

	sigemptyset(&sigact.sa_mask);

	if (sigaction(signal, &sigact, NULL) == -1) {
		throw(UnknownError);
	}
}

/* Register all system signals on startup. */
Constructor {
	scall(registerSystemSignal, ref(Type_BusError));
	scall(registerSystemSignal, ref(Type_ArithmeticError));
	scall(registerSystemSignal, ref(Type_IllegalInstruction));
	scall(registerSystemSignal, ref(Type_InvalidMemoryAccess));
	scall(registerSystemSignal, ref(Type_Alarm));
	scall(registerSystemSignal, ref(Type_Pipe));
}
