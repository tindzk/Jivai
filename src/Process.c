#import "Process.h"
#import "UniStd.h"

#define self Process

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
	int status;
	__unused int result = waitpid(pid, &status, WNOHANG);

	assert(result == pid);
}

def(pid_t, spawn) {
	pid_t pid = fork();

	if (pid == -1) {
		throw(ForkFailed);
	}

	if (pid == 0) { /* child */
		if (this->stdOut != -1) {
			dup2(this->stdOut, ChannelId_StdOut);
		}

		char *argv[this->params->len + 2];
		argv[0] = String_ToNulHeap(this->cmd.rd);
		size_t i;
		for (i = 0; i < this->params->len; i++) {
			argv[i + 1] = String_ToNulHeap(this->params->buf[i].rd);
		}
		argv[i + 1] = NULL;

		if (execve(argv[0], argv, environ) == -1) {
			for (size_t i = 0; argv[i] != NULL; i++) {
				Memory_Destroy(argv[i]);
				argv[i] = NULL;
			}

			throw(SpawningProcessFailed);
		}

		for (size_t i = 0; argv[i] != NULL; i++) {
			Memory_Destroy(argv[i]);
		}

		_exit(127);
	}

	return pid;
}
