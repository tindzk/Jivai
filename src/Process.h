#import <sys/wait.h>
#import <sys/types.h>

#import "UniStd.h"
#import "String.h"
#import "Exception.h"

#undef self
#define self Process

enum {
	excForkFailed = excOffset,
	excSpawningProcessFailed
};

extern char **environ;

class {
	String cmd;
	StringArray *params;
};

ExtendClass(self);

def(void, Init, String cmd);
def(void, Destroy);
def(void, AddParameter, String param);
def(String, GetCommandLine);
overload def(int, Spawn, float *time);
overload def(int, Spawn);
