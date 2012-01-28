#define _GNU_SOURCE
#include <sched.h> /* clone() */

#import "String.h"
#import "Signal.h"
#import "Exception.h"

#define self Process

/* Stack size for the child process before it executes the process. */
#ifndef Process_StackSize
#define Process_StackSize (2 * 1024)
#endif

enum {
	ref(SpawningProcessFailed) = 255
};

set(ref(ChannelId)) {
	ref(ChannelId_Ignore)  = -2,
	ref(ChannelId_Default) = -1

	/* All other values must be valid channel IDs which will be
	 * connected accordingly.
	 */
};

set(ref(Pipe)) {
	ref(Pipe_Disabled),
	ref(Pipe_Read),
	ref(Pipe_Write),
	ref(Pipe_Bidirectional)
};

set(ref(PipeFd)) {
	ref(PipeFd_Read)  = 0,
	ref(PipeFd_Write) = 1
};

exc(ForkFailed)
exc(UnknownError)

extern char **environ;

struct ref(Data);

class {
	String cmd;
	StringArray *params;

	ref(ChannelId) stdIn;
	ref(ChannelId) stdOut;
	ref(ChannelId) stdErr;

	ref(Pipe) pipe;

	struct ref(Data) *data;

	char **args;
};

rsdef(self, new, RdString cmd);
def(void, destroy);
def(struct ref(Data) *, getData);
def(void, addParameter, RdString param);
def(String, getCommandLine);
def(void, setPipe, ref(Pipe) value);
def(void, mapStdIn, ref(ChannelId) value);
def(void, mapStdOut, ref(ChannelId) value);
def(void, mapStdErr, ref(ChannelId) value);
sdef(void, terminate, pid_t pid);
sdef(bool, suspend, pid_t pid);
sdef(void, ignoreId, int id);
sdef(void, close, int pipefd[2], int close);
sdef(void, redirect, int id, int targetId);
def(void, apply, int id, ref(ChannelId) target);
def(struct ref(Data) *, spawn);

#undef self

#define self Process_Data

class {
	Process *context;
	Channel ch;
	int pipefd[2]; /* Access elements via ref(PipeFd) constants. */
	void *stack;
	pid_t pid;
};

def(void, destroy);

#undef self
