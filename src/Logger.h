#import "String.h"
#import "Bit.h"
#import "BitMask.h"

#ifndef Logger_DisabledLevels
#define Logger_DisabledLevels 0
#endif

typedef enum {
	Logger_Level_Fatal = Bit(0),
	Logger_Level_Crit  = Bit(1),
	Logger_Level_Error = Bit(2),
	Logger_Level_Warn  = Bit(3),
	Logger_Level_Info  = Bit(4),
	Logger_Level_Debug = Bit(5),
	Logger_Level_Trace = Bit(6)
} Logger_Level;

typedef void (* Logger_Printer)(void *, String, Logger_Level, String, int);

typedef struct {
	void *context;
	Logger_Printer printer;

	int levels;
} Logger;

void Logger_Init(Logger *this, Logger_Printer printer, void *context, int levels);
String Logger_LevelToString(Logger_Level level);

#define Logger_IsEnabled(this, level) \
	(!BitMask_Has(Logger_DisabledLevels, level) && BitMask_Has((this)->levels, level))

#define Logger_Log(this, level, msg)                     \
	do {                                                 \
		if (Logger_IsEnabled(this, level)) {             \
			(this)->printer((this)->context,             \
				msg, level, String(__FILE__), __LINE__); \
		}                                                \
	} while(0)

#define Logger_LogFmt(this, level, fmt, ...)                   \
	do {                                                       \
		if (Logger_IsEnabled(this, level)) {                   \
			String __fmt = String_Format(fmt, __VA_ARGS__);    \
			(this)->printer((this)->context,                   \
					__fmt, level, String(__FILE__), __LINE__); \
			Memory_Free(__fmt.buf);                            \
		}                                                      \
	} while(0)
