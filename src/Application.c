#import "Application.h"

#define self Application

def(void, Init, int argc, char *argv[], char *envp[]) {
	FPU_setExceptions(FPU_Exception_All & ~FPU_Exception_InexactResult);

	size_t envItems = 0;
	for (char **cur = envp; *cur != NULL; cur++) {
		envItems++;
	}

	this->base   = String_FromNul(argv[0]);
	this->env    = RdStringArray_New(envItems);
	this->args   = RdStringArray_New(argc - 1);
	this->logger = Logger_New(Logger_Printer_For(this, ref(OnLogMessage)));
	this->term   = Terminal_New(false);
	Terminal_SetOutput(&this->term, Channel_StdErr);

	for (int i = 1; i < argc; i++) {
		RdStringArray_Push(&this->args, String_FromNul(argv[i]));
	}

	fwd(i, envItems) {
		RdStringArray_Push(&this->env, String_FromNul(envp[i]));
	}
}

def(void, Destroy) {
	RdStringArray_Free(this->args);
	RdStringArray_Free(this->env);
	Terminal_Destroy(&this->term);
}

override def(void, OnLogMessage, FmtString msg, Logger_Level level, RdString file, int line) {
	RdString slevel = Logger_ResolveLevel(level);
	String sline = Integer_ToString(line);

	Terminal_FmtPrint(&this->term,
		$("[%] $ (%:%)\n"),
		slevel, msg, file, sline.rd);

	String_Destroy(&sline);
}

static Memory_Map    map;
static Memory_Libc   libc;
static Memory_Logger logger;
static bool configured = false; /* TODO earlyConstructor doesn't work yet. */

override earlyConstructor void configureMemory(void) {
	if (configured) {
		return;
	}

	if (1 || System_IsRunningOnValgrind()) {
		String_Print($("[Info] Running in Valgrind. Using traditional allocator functions.\n"));
		libc = Memory_Libc_New();
		Memory0(Memory_Libc_AsMemory(&libc));
	} else if (System_IsDebugging()) {
		map    = Memory_Map_New();
		logger = Memory_Logger_New(Memory_Map_AsMemory(&map));

		Memory0(Memory_Logger_AsMemory(&logger));
	} else {
		map = Memory_Map_New();
		Memory0(Memory_Map_AsMemory(&map));
	}

	configured = true;
}
