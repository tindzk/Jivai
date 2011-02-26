#import "Logger.h"

#define self Logger

def(void, Init, ref(Printer) printer, int levels) {
	this->printer = printer;
	this->levels  = levels;
}

ProtString ref(ResolveLevel)(ref(Level) level) {
	switch (level) {
		case ref(Level_Fatal):
			return $("Fatal");

		case ref(Level_Crit):
			return $("Crit");

		case ref(Level_Error):
			return $("Error");

		case ref(Level_Warn):
			return $("Warn");

		case ref(Level_Info):
			return $("Info");

		case ref(Level_Debug):
			return $("Debug");

		case ref(Level_Trace):
			return $("Trace");

		default:
			return $("Unknown");
	}
}
