#import "Main.h"

int main(int argc, char *argv[], char *envp[]) {
	Signal0();

	size_t envItems = 0;
	for (char **cur = envp; *cur != NULL; cur++) {
		envItems++;
	}

	ProtString       base = String_FromNul(argv[0]);
	ProtStringArray *env  = ProtStringArray_New(envItems);
	ProtStringArray *args = ProtStringArray_New(argc - 1);

	for (int i = 1; i < argc; i++) {
		ProtStringArray_Push(&args, String_FromNul(argv[i]));
	}

	forward (i, envItems) {
		ProtStringArray_Push(&env, String_FromNul(envp[i]));
	}

	int ret = ExitStatus_Success;

	try {
		ret = Main(base, args, env)
			? ExitStatus_Success
			: ExitStatus_Failure;
	} catch(Signal, SigInt) {

	} catchAny {
		Exception_Print(e);

#if Exception_SaveTrace
		Backtrace_PrintTrace(
			Exception_GetTraceBuffer(),
			Exception_GetTraceLength());
#endif

		ret = ExitStatus_Failure;
	} finally {
		ProtStringArray_Free(args);
		ProtStringArray_Free(env);
	} tryEnd;

	return ret;
}
