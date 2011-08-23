#import "String.h"
#import "Exception.h"

#define self Process

#define Process_SpawningProcessFailed 255

exc(ForkFailed)

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
sdef(void, suspend, pid_t pid);
def(pid_t, spawn);

#undef self
