#import "Process.h"

#define self Process

/* Resources:
 * http://stackoverflow.com/questions/4259629/what-is-the-difference-between-fork-and-vfork/5207945#5207945
 * http://blog.famzah.net/2009/11/20/fork-gets-slower-as-parent-process-use-more-memory/
 * http://developers.sun.com/solaris/articles/subprocess/subprocess.html
 *
 * clone() is used in order to increase performance. Code adapted from:
 *   http://blog.famzah.net/2009/11/20/a-much-faster-popen-and-system-implementation-for-linux/
 *   http://code.google.com/p/popen-noshell/source/browse/trunk/popen_noshell.c
 */

rsdef(self, new, RdString cmd) {
	return (self) {
		.cmd    = String_Clone(cmd),
		.params = StringArray_New(0),
		.stdOut = ref(ChannelId_Default),
		.stdIn  = ref(ChannelId_Default),
		.stdErr = ref(ChannelId_Default)
	};
}

static def(void, freeArgs, char **args) {
	for (size_t i = 0; args[i] != null; i++) {
		Memory_Destroy(args[i]);
	}

	Memory_Destroy(args);
}

def(void, destroy) {
	if (this->args != null) {
		call(freeArgs, this->args);
	}

	String_Destroy(&this->cmd);

	StringArray_Destroy(this->params);
	StringArray_Free(this->params);
}

def(struct ref(Data) *, getData) {
	return this->data;
}

def(void, addParameter, RdString param) {
	StringArray_Push(&this->params, String_Clone(param));
}

def(String, getCommandLine) {
	String out = String_New(128);

	String_Append(&out, this->cmd.rd);
	String_Append(&out, ' ');

	each(param, this->params) {
		String_Append(&out, param->rd);

		if (!isLast(param, this->params)) {
			String_Append(&out, ' ');
		}
	}

	return out;
}

def(void, setPipe, ref(Pipe) value) {
	assert(this->stdIn  == ref(ChannelId_Default));
	assert(this->stdOut == ref(ChannelId_Default));
	assert(this->stdErr == ref(ChannelId_Default));

	this->pipe = value;
}

def(void, mapStdIn, ref(ChannelId) value) {
	assert(this->pipe == ref(Pipe_Disabled));
	this->stdIn = value;
}

def(void, mapStdOut, ref(ChannelId) value) {
	assert(this->pipe == ref(Pipe_Disabled));
	this->stdOut = value;
}

def(void, mapStdErr, ref(ChannelId) value) {
	assert(this->pipe == ref(Pipe_Disabled));
	this->stdErr = value;
}

/* Terminates a process. The kernel will automatically generate a
 * HangUp event afterwards.
 */
sdef(void, terminate, pid_t pid) {
	int result = kill(pid, Signal_Type_Terminate);
	assert(result == 0);
}

/* Free allocated resources of a zombie. */
sdef(bool, suspend, pid_t pid) {
	assert(pid > 0);

	int status;
	int result = Kernel_waitpid(pid, &status, WNOHANG);

	if (result == 0) {
		/* Process is still running. */
		return false;
	}

	assert(result == pid);
	return true;
}

static def(char **, getArgs) {
	char **argv = Memory_New((this->params->len + 2) * sizeof(char *));

	argv[0] = String_ToNulHeap(this->cmd.rd);

	size_t i;

	for (i = 0; i < this->params->len; i++) {
		argv[i + 1] = String_ToNulHeap(this->params->buf[i].rd);
	}

	argv[i + 1] = null;

	return argv;
}

sdef(void, ignoreId, int id) {
	int newId = Kernel_open($("/dev/null"), FileStatus_ReadWrite, 0);

	(newId != -1)
		|| throw(UnknownError);

	assert(id != -1);

	Kernel_close(id)
		|| throw(UnknownError);

	Kernel_dup2(newId, id)
		|| throw(UnknownError);

	Kernel_close(newId)
		|| throw(UnknownError);
}

sdef(void, close, int pipefd[2], int close) {
	assert(pipefd[close] != -1);

	Kernel_close(pipefd[close])
		|| throw(UnknownError);

	pipefd[close] = -1;
}

sdef(void, redirect, int id, int targetId) {
	assert(id       != -1);
	assert(targetId != -1);

	Kernel_close(targetId)
		|| throw(UnknownError);

	Kernel_dup2(id, targetId)
		|| throw(UnknownError);

	Kernel_close(id)
		|| throw(UnknownError);
}

def(void, apply, int id, ref(ChannelId) target) {
	if (target == ref(ChannelId_Ignore)) {
		scall(ignoreId, id);
	} else if (target != ref(ChannelId_Default)) {
		Kernel_dup2(target, id)
			|| throw(UnknownError);
	}
}

static def(int, childProcess) {
	if (this->pipe == ref(Pipe_Read)) {
		scall(ignoreId, ChannelId_StdIn);

		/* Connect the pipe's write end to stdOut. */
		scall(redirect, this->data->pipefd[ref(PipeFd_Write)], ChannelId_StdOut);

		/* Close unused read end of the pipe. */
		scall(close, this->data->pipefd, ref(PipeFd_Read));
	} else if (this->pipe == ref(Pipe_Write)) {
		scall(ignoreId, ChannelId_StdOut);

		/* Connect the pipe's read end to stdIn. */
		scall(redirect, this->data->pipefd[ref(PipeFd_Read)], ChannelId_StdIn);

		/* Close unused write end of the pipe. */
		scall(close, this->data->pipefd, ref(PipeFd_Write));
	} else if (this->pipe == ref(Pipe_Disabled)) {
		call(apply, ChannelId_StdIn,  this->stdIn);
		call(apply, ChannelId_StdOut, this->stdOut);
		call(apply, ChannelId_StdErr, this->stdErr);
	} else {
		/* Bidirectional streams not implemented yet. */
		assert(false);
	}

	/* execve() does not return on success. */
	Kernel_execve(this->cmd.rd, this->args, environ);

	Kernel_close(ChannelId_StdIn);
	Kernel_close(ChannelId_StdOut);
	Kernel_close(ChannelId_StdErr);

	/* Exit code for the child process. */
	return ref(SpawningProcessFailed);
}

static def(pid_t, krnFork) {
	pid_t pid = Kernel_fork();

	if (pid == -1) {
		throw(ForkFailed);
	} else if (pid == 0) { /* Child. */
		_exit(call(childProcess));
	} else { /* Parent. */
		return pid;
	}
}

/* Similar to vfork() and threading. Starts a process which behaves
 * like a thread (shares global variables in memory with the parent),
 * but has a different PID and can call execve(), unlike traditional
 * threads which are not allowed to call execve(). This method consumes
 * fewer resources than fork() as it does not copy any memory from the
 * parent, but shares it. Throws an exception when an error occurs.
 * Returns the PID of the newly created child upon success.
 */
static def(pid_t, cloneFork) {
	this->data->stack = Memory_New(ref(StackSize) + 15);

	/* On all supported Linux platforms the stack grows downward,
	 * except for HP-PARISC. You can grep the kernel source for
	 * `STACK_GROWSUP' in order to get this information. Set the
	 * pointer to the end of the block.
	 */
	void *stackAligned = (byte *) this->data->stack + ref(StackSize);

	/* On all supported platforms by GNU libc, the stack is aligned to
	 * 16 bytes, except for the SuperH platform which is aligned to
	 * 8 bytes. You can grep the glibc source for `STACK_ALIGN' in
	 * order to get this information.
	 * See also http://stackoverflow.com/questions/227897/solve-the-memory-alignment-in-c-interview-question-that-stumped-me
	 * Round up to multiple of 16: Add 15 and then round down by
	 * masking.
	 */
	stackAligned = (void *) (((IntPtr) stackAligned + 15) & -16);

	/* `this' must be cloned because it may be stack-allocated. */
	this->data->context = Memory_New(sizeof(self));
	Memory_Copy(this->data->context, this, sizeof(self));

	/* The child terminates when childProcess() returns. */
	pid_t pid = clone((int (*)(void *)) ref(childProcess),
				stackAligned,
				CLONE_VM | Signal_Type_ChildStatus,
				this->data->context);

	if (pid == -1) {
		throw(ForkFailed);
	} else if (pid == 0) { /* Child. */
		throw(UnknownError);
	} else { /* Parent. */
		return pid;
	}
}

def(struct ref(Data) *, spawn) {
	/* An instance of this class can only spawn a process once. */
	assert(this->data == null);
	assert(this->args == null);

	this->data = Memory_New(sizeof(ref(Data)));
	this->args = call(getArgs);

	this->data->stack   = null;
	this->data->context = null;

	if (this->pipe != ref(Pipe_Disabled)) {
		if (pipe2(this->data->pipefd, FileStatus_NonBlock) != 0) {
			throw(UnknownError);
		}

		if (this->pipe == ref(Pipe_Bidirectional)) {
			/* @todo A second pipe2() call is necessary for bidirectional
			 * communication with the process. Currently we can only either
			 * read or write to the process.
			 */
		}
	}

	if (1 || System_IsRunningOnValgrind()) {
		this->data->pid = call(krnFork);
	} else {
		/* @todo cloneFork() is unreliable when spawning multiple processes. */
		this->data->pid = call(cloneFork);
	}

	/* Parent process. */

	if (this->pipe == ref(Pipe_Read)) {
		this->data->ch = Channel_New(this->data->pipefd[ref(PipeFd_Read)],
			FileStatus_ReadOnly | FileStatus_NonBlock);
	} else if (this->pipe == ref(Pipe_Write)) {
		this->data->ch = Channel_New(this->data->pipefd[ref(PipeFd_Write)],
			FileStatus_WriteOnly | FileStatus_NonBlock);
	} else if (this->pipe == ref(Pipe_Bidirectional)) {
		/* @todo Implement. */
	} else {
		this->data->ch = Channel_New(-1, 0);
	}

	return this->data;
}

#undef self

#define self Process_Data

def(void, destroy) {
	if (this->stack != null) {
		Memory_Destroy(this->stack);
	}

	if (this->context != null) {
		Memory_Destroy(this->context);
	}

	if (this->pipefd[Process_PipeFd_Read] != -1) {
		Process_close(this->pipefd, Process_PipeFd_Read);
	}

	if (this->pipefd[Process_PipeFd_Write] != -1) {
		Process_close(this->pipefd, Process_PipeFd_Write);
	}

	Process_suspend(this->pid);

	Memory_Destroy(this);
}
