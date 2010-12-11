#import <sys/wait.h>
#import <sys/types.h>

#import "UniStd.h"
#import "String.h"
#import "Exception.h"

#define self Process

// @exc ForkFailed
// @exc SpawningProcessFailed

extern char **environ;

class {
	String cmd;
	StringArray *params;
};

def(void, Init, String cmd);
def(void, Destroy);
def(void, AddParameter, String param);
def(String, GetCommandLine);
overload def(int, Spawn, float *time);
overload def(int, Spawn);

#undef self
