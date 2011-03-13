#import "Logger.h"

#define self Logger

static RdString levels[] = {
	[ref(Level_Fatal)] = $("Fatal"),
	[ref(Level_Crit)]  = $("Crit"),
	[ref(Level_Error)] = $("Error"),
	[ref(Level_Warn)]  = $("Warn"),
	[ref(Level_Info)]  = $("Info"),
	[ref(Level_Debug)] = $("Debug"),
	[ref(Level_Trace)] = $("Trace")
};

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

def(void, SetLevels, int levels) {
	this->levels = levels;
}

sdef(RdString, ResolveLevel, ref(Level) level) {
	if (level > ref(Level_Trace)) {
		return $("Unknown");
	}

	return levels[level];
}
