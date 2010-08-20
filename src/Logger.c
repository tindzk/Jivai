#import "Logger.h"

void Logger_Init(Logger *this, Logger_Printer printer, void *context, int levels) {
	this->printer = printer;
	this->context = context;
	this->levels  = levels;
}

String Logger_LevelToString(Logger_Level level) {
	switch (level) {
		case Logger_Level_Fatal:
			return String("Fatal");

		case Logger_Level_Crit:
			return String("Crit");

		case Logger_Level_Error:
			return String("Error");

		case Logger_Level_Warn:
			return String("Warn");

		case Logger_Level_Info:
			return String("Info");

		case Logger_Level_Debug:
			return String("Debug");

		case Logger_Level_Trace:
			return String("Trace");

		default:
			return String("Unknown");
	}
}
