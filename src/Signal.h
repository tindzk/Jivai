#ifndef SIGNAL_H
#define SIGNAL_H

#include <signal.h>

#include "String.h"
#include "Compiler.h"
#include "Exception.h"

Exception_Export(Signal_SigAlrmException);
Exception_Export(Signal_SigBusException);
Exception_Export(Signal_SigFpeException);
Exception_Export(Signal_SigIllException);
Exception_Export(Signal_SigIntException);
Exception_Export(Signal_SigPipeException);
Exception_Export(Signal_SigQuitException);
Exception_Export(Signal_SigSegvException);
Exception_Export(Signal_SigTermException);
Exception_Export(Signal_SignalHandlerNotSetException);
Exception_Export(Signal_UnknownException);

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

#endif
