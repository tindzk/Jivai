#import "Bit.h"
#import "String.h"
#import "BitMask.h"

#define self Logger

#ifndef Logger_DisabledLevels
#define Logger_DisabledLevels 0
#endif

set(ref(Level)) {
	ref(Level_Fatal) = Bit(0),
	ref(Level_Crit)  = Bit(1),
	ref(Level_Error) = Bit(2),
	ref(Level_Warn)  = Bit(3),
	ref(Level_Info)  = Bit(4),
	ref(Level_Debug) = Bit(5),
	ref(Level_Trace) = Bit(6),
	ref(Level_Count) = 7
};

DefineCallback(ref(Printer), void, FmtString msg, ref(Level) level, ProtString file, int line);

class {
	ref(Printer) printer;
	int levels;
};

static inline def(bool, IsEnabled, ref(Level) level) {
	return !BitMask_Has(Logger_DisabledLevels, level)
		 && BitMask_Has(this->levels, level);
}

def(void, Init, ref(Printer) printer, int levels);
ProtString ref(ResolveLevel)(ref(Level) level);

#define Logger_Log(this, level, fmt, ...)       \
	do {                                        \
		if (Logger_IsEnabled(this, level)) {    \
			callback((this)->printer,           \
				FmtString(fmt, ## __VA_ARGS__), \
				level, $(__FILE__), __LINE__);  \
		}                                       \
	} while(0)

#define Logger_Fatal(this, fmt, ...) \
	Logger_Log(this, Logger_Level_Fatal, fmt, ## __VA_ARGS__)

#define Logger_Crit(this, fmt, ...) \
	Logger_Log(this, Logger_Level_Crit, fmt, ## __VA_ARGS__)

#define Logger_Error(this, fmt, ...) \
	Logger_Log(this, Logger_Level_Error, fmt, ## __VA_ARGS__)

#define Logger_Warn(this, fmt, ...) \
	Logger_Log(this, Logger_Level_Warn, fmt, ## __VA_ARGS__)

#define Logger_Info(this, fmt, ...) \
	Logger_Log(this, Logger_Level_Info, fmt, ## __VA_ARGS__)

#define Logger_Debug(this, fmt, ...) \
	Logger_Log(this, Logger_Level_Debug, fmt, ## __VA_ARGS__)

#define Logger_Trace(this, fmt, ...) \
	Logger_Log(this, Logger_Level_Trace, fmt, ## __VA_ARGS__)

#undef self
