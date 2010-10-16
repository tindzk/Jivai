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
	Logger_Level_Trace = Bit(6),
	Logger_Level_Count = 7
} Logger_Level;

typedef void (* Logger_Printer)(void *, String, Logger_Level, String, int);

typedef struct {
	void *context;
	Logger_Printer printer;

	int levels;
} Logger;

void Logger_Init(Logger *this, Logger_Printer printer, void *context, int levels);
bool Logger_IsEnabled(Logger *this, Logger_Level level);
String Logger_LevelToString(Logger_Level level);

#define Logger_Log(this, level, fmt, ...)                      \
	do {                                                       \
		if (Logger_IsEnabled(this, level)) {                   \
			String __fmt = String_Format(fmt, ## __VA_ARGS__); \
			(this)->printer((this)->context,                   \
				__fmt, level, String(__FILE__), __LINE__);     \
			Memory_Free(__fmt.buf);                            \
		}                                                      \
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
