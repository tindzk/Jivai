#import <String.h>
#import <Logger.h>
#import <Integer.h>

#undef Logger_DisabledLevels
#define Logger_DisabledLevels Logger_Level_Debug

void onLogMessage(__unused void *ptr, String msg, Logger_Level level, String file, int line) {
	String slevel = Logger_ResolveLevel(level);
	String sline  = Int32_ToString(line);

	String tmp;
	String_Print(tmp = String_Format(
		$("[%] % (%:%)\n"),
		slevel, msg, file, sline));

	String_Destroy(&tmp);
}

int main(void) {
	Logger logger;

	/* Log everything. */
	Logger_Init(&logger, Callback(NULL, onLogMessage),
		Logger_Level_Fatal |
		Logger_Level_Crit  |
		Logger_Level_Error |
		Logger_Level_Warn  |
		Logger_Level_Info  |
		Logger_Level_Debug |
		Logger_Level_Trace);

	Logger_Log(&logger, Logger_Level_Info, $("Application started."));

	/* Will not be shown because the application is compiled without
	 * debug messages. Hence, enabling the flag Logger_Level_Debug
	 * won't change anything. */
	Logger_Log(&logger, Logger_Level_Debug, $("Ignored message."));

	/* Print a trace message. */
	Logger_Log(&logger, Logger_Level_Trace, $("Trace message 1."));

	/* Now disable trace messages. */
	BitMask_Clear(logger.levels, Logger_Level_Trace);

	/* This will not be shown. */
	Logger_Log(&logger, Logger_Level_Trace, $("Trace message 2."));

	/* Logging formatted messages is possible, too. */
	Logger_Log(&logger, Logger_Level_Info, $("Stopping %..."), $("application"));

	return ExitStatus_Success;
}
