#import <sys/wait.h>
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

rsdef(self, New, RdString cmd);
def(void, Destroy);
def(void, AddParameter, RdString param);
def(String, GetCommandLine);
def(void, MapStdOut, int fd);
overload def(int, Spawn, float *time);
overload def(int, Spawn);

#undef self
