#import "Main.h"

int main(int argc, char *argv[], char *envp[]) {
	Signal0();

	Application app;
	Application_Init(&app, argc, argv, envp);

	int ret = ExitStatus_Success;

	try {
		ret = Application_Run(&app)
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
		Application_Destroy(&app);
	} tryEnd;

	return ret;
}
