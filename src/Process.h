#import <sys/wait.h>
#import <sys/types.h>

#import "UniStd.h"
#import "String.h"
#import "ExceptionManager.h"

#undef self
#define self Process

enum {
	excForkFailed = excOffset,
	excSpawningProcessFailed
};

extern char **environ;

typedef struct {
	String cmd;
	StringArray *params;
} Process;

void Process_Init(Process *this, String cmd);
void Process_Destroy(Process *this);
void Process_AddParameter(Process *this, String param);
String Process_GetCommandLine(Process *this);
overload int Process_Spawn(Process *this, float *time);
overload int Process_Spawn(Process *this);
