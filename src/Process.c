#import "Process.h"

#define self Process

/*
 * Copyright (C) 2008-2010 vmchecker authors and contributors.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *
 * This file is based upon vmchecker/testing/pa/tracker.c by Claudiu Gheorghe.
 */

def(void, Init, ProtString cmd) {
	this->cmd    = String_Clone(cmd);
	this->params = StringArray_New(0);
	this->stdOut = -1;
}

def(void, Destroy) {
	String_Destroy(&this->cmd);

	StringArray_Destroy(this->params);
	StringArray_Free(this->params);
}

def(void, AddParameter, ProtString param) {
	StringArray_Push(&this->params, String_Clone(param));
}

def(String, GetCommandLine) {
	String out = String_New(128);

	String_Append(&out, this->cmd.prot);
	String_Append(&out, ' ');

	each(param, this->params) {
		String_Append(&out, param->prot);

		if (!isLast(param, this->params)) {
			String_Append(&out, ' ');
		}
	}

	return out;
}

def(void, MapStdOut, int fd) {
	this->stdOut = fd;
}

overload def(int, Spawn, float *time) {
	char *argv[this->params->len + 1];

	argv[0] = String_ToNulHeap(this->cmd.prot);

	size_t i;

	for (i = 0; i < this->params->len; i++) {
		argv[i + 1] = String_ToNulHeap(this->params->buf[i].prot);
	}

	argv[i + 1] = NULL;

	pid_t pid, w;
	int ret = -1;
	int status;

	if ((pid = fork()) == 0) { /* child */
		if (this->stdOut != -1) {
			dup2(this->stdOut, FileNo_StdOut);
		}

		if (execve(argv[0], argv, environ) < 0) {
			for (size_t i = 0; argv[i] != NULL; i++) {
				Pool_Free(Pool_GetInstance(), argv[i]);
				argv[i] = NULL;
			}

			throw(SpawningProcessFailed);
		}

		_exit(127);
	} else if (pid > 0) { /* parent */
		struct rusage accounting;
		w = wait4(pid, &status, 0, &accounting);

		if (WEXITSTATUS(status)) {
			return WEXITSTATUS(status);
		} else {
			if (time != NULL) {
				int seconds  = (int) accounting.ru_utime.tv_sec  + (int) accounting.ru_stime.tv_sec;
				int useconds = (int) accounting.ru_utime.tv_usec + (int) accounting.ru_stime.tv_usec;

				*time = (float) seconds + (float) useconds / 1000000;
			}
		}
	} else {
		for (size_t i = 0; argv[i] != NULL; i++) {
			Pool_Free(Pool_GetInstance(), argv[i]);
			argv[i] = NULL;
		}

		throw(ForkFailed);
	}

	for (size_t i = 0; argv[i] != NULL; i++) {
		Pool_Free(Pool_GetInstance(), argv[i]);
	}

	return ret;
}

overload def(int, Spawn) {
	return call(Spawn, NULL);
}
