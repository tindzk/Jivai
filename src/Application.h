#define _GNU_SOURCE
#import <link.h> /* dl_iterate_phdr() */
#undef _GNU_SOURCE

#import "FPU.h"
#import "Folder.h"
#import "String.h"
#import "Locale.h"
#import "Logger.h"
#import "Terminal.h"
#import "Memory/Map.h"
#import "Memory/Libc.h"
#import "Memory/Logger.h"

#define self Application

class {
	RdString      base;
	RdStringArray *env;
	RdStringArray *args;
	Terminal      term;
	Logger        logger;
};

def(void, Init, int argc, char *argv[], char *envp[]);
def(void, Destroy);
def(void, OnLogMessage, FmtString msg, Logger_Level level, RdString file, int line);
def(bool, Run);
earlyConstructor void configureMemory(void);

#undef self
