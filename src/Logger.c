#import "Logger.h"

#define self Logger

rsdef(self, New, ref(Printer) printer) {
	return (self) {
		.printer = printer,
		.levels  =
			Logger_Level_Fatal |
			Logger_Level_Crit  |
			Logger_Level_Error |
			Logger_Level_Warn  |
			Logger_Level_Info  |
			Logger_Level_Debug |
			Logger_Level_Trace
	};
}

def(void, SetLevels, int value) {
	this->levels = value;
}

sdef(RdString, ResolveLevel, ref(Level) level) {
	if (level == ref(Level_Fatal)) {
		return t("Fatal");
	} else if (level == ref(Level_Crit)) {
		return t("Crit");
	} else if (level == ref(Level_Error)) {
		return t("Error");
	} else if (level == ref(Level_Warn)) {
		return t("Warn");
	} else if (level == ref(Level_Info)) {
		return t("Info");
	} else if (level == ref(Level_Debug)) {
		return t("Debug");
	} else if (level == ref(Level_Trace)) {
		return t("Trace");
	} else {
		return t("Unknown");
	}
}
