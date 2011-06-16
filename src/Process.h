#import <sys/types.h>

#import "String.h"
#import "Exception.h"

#define self Process

// @exc ForkFailed
// @exc SpawningProcessFailed

extern char **environ;

class {
	String cmd;
	StringArray *params;
	int stdOut;
};

rsdef(self, new, RdString cmd);
def(void, destroy);
def(void, addParameter, RdString param);
def(String, getCommandLine);
def(void, mapStdOut, int fd);
def(pid_t, spawn);

#undef self
