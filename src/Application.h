#import "String.h"
#import "Logger.h"
#import "Terminal.h"

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
def(void, OnLogMessage, FmtString msg, Logger_Level level, String file, int line);
def(bool, Run);

#undef self
