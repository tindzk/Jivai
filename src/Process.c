#import "Process.h"

#define self Process

/* Resources:
 * http://stackoverflow.com/questions/4259629/what-is-the-difference-between-fork-and-vfork/5207945#5207945
 * http://blog.famzah.net/2009/11/20/fork-gets-slower-as-parent-process-use-more-memory/
 * http://developers.sun.com/solaris/articles/subprocess/subprocess.html
 */

rsdef(self, new, RdString cmd) {
	return (self) {
		.cmd    = String_Clone(cmd),
		.params = StringArray_New(0),
		.stdOut = -1
	};
}

def(void, destroy) {
	String_Destroy(&this->cmd);

	StringArray_Destroy(this->params);
	StringArray_Free(this->params);
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

def(void, mapStdOut, int fd) {
	this->stdOut = fd;
}

sdef(void, suspend, pid_t pid) {
	assert(pid > 0);

	int status;
	__unused int result = Kernel_waitpid(pid, &status, WNOHANG);

	assert(result == pid);
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

static def(void, freeArgs, char **args) {
	for (size_t i = 0; args[i] != null; i++) {
		Memory_Destroy(args[i]);
	}

	Memory_Destroy(args);
}

/*
 * @todo Use clone() for better performance:
 *       http://blog.famzah.net/2009/11/20/a-much-faster-popen-and-system-implementation-for-linux/
 *       http://code.google.com/p/popen-noshell/source/browse/trunk/popen_noshell.c
 */
def(pid_t, spawn) {
	/* Should not be called in child. Otherwise, its memory could only
	 * be freed when execve() fails.
	 */
	char **args = call(getArgs);

	pid_t pid = Kernel_fork();

	if (pid == -1) {
		throw(ForkFailed);
	}

	if (pid == 0) { /* child */
		if (this->stdOut != -1) {
			Kernel_dup2(this->stdOut, ChannelId_StdOut);
		}

		if (!Kernel_execve(this->cmd.rd, args, environ)) {
			_exit(ref(SpawningProcessFailed));
		}

		/* execve() does not return on success. */
	} else { /* parent */
		call(freeArgs, args);
	}

	return pid;
}
