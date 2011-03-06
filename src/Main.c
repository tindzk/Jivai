#import "Main.h"

int main(int argc, char *argv[]) {
	Signal0();

	ProtString       base = String_FromNul(argv[0]);
	ProtStringArray *args = ProtStringArray_New(argc - 1);

	for (int i = 1; i < argc; i++) {
		ProtStringArray_Push(&args, String_FromNul(argv[i]));
	}

	int ret = ExitStatus_Success;

	try {
		ret = Main(base, args)
			? ExitStatus_Success
			: ExitStatus_Failure;
	} clean catch(Signal, SigInt) {

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
	} tryEnd;

	return ret;
}
