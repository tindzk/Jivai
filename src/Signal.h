#import <signal.h>
#import <sys/signalfd.h>

#import "Kernel.h"
#import "String.h"
#import "Channel.h"
#import "Compiler.h"
#import "EventLoop.h"
#import "Exception.h"

#define self Signal

// @exc Alarm
// @exc ArithmeticError
// @exc BusError
// @exc IllegalInstruction
// @exc InvalidMemoryAccess
// @exc Pipe
// @exc UnknownError

/* This structure mirrors the one found in /usr/include/asm/ucontext.h */
record(ref(UserContext)) {
	unsigned long     uc_flags;
	struct ucontext   *uc_link;
	stack_t           uc_stack;
	struct sigcontext uc_mcontext;
	sigset_t          uc_sigmask;
};

set(ref(Type)) {
	ref(Type_Terminate)           = 15,
	ref(Type_ChildStatus)         = 17,
	ref(Type_Interrupt)           = 2,
	ref(Type_Quit)                = 3,
	ref(Type_Kill)                = 9,
	ref(Type_Stop)                = 19,
	ref(Type_HangUp)              = 1,
	ref(Type_Alarm)               = 14,
	ref(Type_BusError)            = 7,
	ref(Type_ArithmeticError)     = 8,
	ref(Type_IllegalInstruction)  = 4,
	ref(Type_InvalidMemoryAccess) = 11,
	ref(Type_Pipe)                = 13
};

/* System signals will be turned into an exception which shouldn't
 * be caught. These signals are dealt with globally and cannot be
 * used for add().
 */
static alwaysInline sdef(bool, isSystemSignal, ref(Type) signal) {
	return signal == ref(Type_BusError)
		|| signal == ref(Type_ArithmeticError)
		|| signal == ref(Type_IllegalInstruction)
		|| signal == ref(Type_InvalidMemoryAccess)
		|| signal == ref(Type_Alarm)
		|| signal == ref(Type_Pipe);
}

/* These are signals that are safe to be caught by the user and can
 * be added using add().
 */
static alwaysInline sdef(bool, isCustomSignal, ref(Type) signal) {
	return !scall(isSystemSignal, signal)
		&& signal != ref(Type_Kill)
		&& signal != ref(Type_Stop);
}

/* Exit signals include:
 *  - Ctrl-C
 *  - kill <pid> and
 *  - terminal hang ups.
 */
static alwaysInline sdef(bool, isExitSignal, ref(Type) signal) {
	return signal == ref(Type_Terminate)
		|| signal == ref(Type_Interrupt)
		|| signal == ref(Type_Quit)
		|| signal == ref(Type_HangUp);
}

Callback(ref(OnCustom),         void, ref(Type) type);
Callback(ref(OnTerminate),      void, ref(Type) type);
Callback(ref(OnChildTerminate), void, pid_t pid, int status);

record(ref(ChildTermination)) {
	pid_t pid;
	ref(OnChildTerminate) cb;
};

Array(ref(ChildTermination), ref(ChildTerminations));

class {
	Channel ch;
	sigset_t mask;

	EventLoop_Entry        *evLoop;
	ref(ChildTerminations) *terminations;

	ref(OnCustom)    onCustom;
	ref(OnTerminate) onTerminate;
};

sdef(void, ignoreSignal, int signal);
rsdef(self, New);
def(void, Destroy);
def(void, listen);
def(void, add, int signal);
def(void, delete, int signal);
def(void, uponCustom, ref(OnCustom) cb);
def(void, uponTermination, ref(OnTerminate) cb);
def(void, uponChildTermination, pid_t pid, ref(OnChildTerminate) cb);

SingletonPrototype(self);

#undef self
