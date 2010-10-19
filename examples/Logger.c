#import <String.h>
#import <Logger.h>
#import <Integer.h>

#undef Logger_DisabledLevels
#define Logger_DisabledLevels Logger_Level_Debug

ExceptionManager exc;

void OnLogMessage(__unused void *ptr, String msg, Logger_Level level, String file, int line) {
	String slevel = Logger_LevelToString(level);
	String sline  = Integer_ToString(line);

	String tmp;
	String_Print(tmp = String_Format(
		String("[%] % (%:%)\n"),
		slevel, msg, file, sline));

	String_Destroy(&tmp);
}

int main(void) {
	ExceptionManager_Init(&exc);

	String0(&exc);

	Logger logger;

	/* Log everything. */
	Logger_Init(&logger, &OnLogMessage, NULL,
		Logger_Level_Fatal |
		Logger_Level_Crit  |
		Logger_Level_Error |
		Logger_Level_Warn  |
		Logger_Level_Info  |
		Logger_Level_Debug |
		Logger_Level_Trace);

	Logger_Log(&logger, Logger_Level_Info, String("Application started."));

	/* Will not be shown because the application is compiled without
	 * debug messages. Hence, enabling the flag Logger_Level_Debug
	 * won't change anything. */
	Logger_Log(&logger, Logger_Level_Debug, String("Ignored message."));

	/* Print a trace message. */
	Logger_Log(&logger, Logger_Level_Trace, String("Trace message 1."));

	/* Now disable trace messages. */
	BitMask_Clear(logger.levels, Logger_Level_Trace);

	/* This will not be shown. */
	Logger_Log(&logger, Logger_Level_Trace, String("Trace message 2."));

	/* Logging formatted messages is possible, too. */
	Logger_Log(&logger, Logger_Level_Info, String("Stopping %..."), String("application"));

	return ExitStatus_Success;
}
