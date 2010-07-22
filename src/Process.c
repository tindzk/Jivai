#include "Process.h"

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

Exception_Define(Process_ForkFailedException);
Exception_Define(Process_SpawningProcessFailedException);

static ExceptionManager *exc;

void Process0(ExceptionManager *e) {
	exc = e;
}

void Process_Init(Process *this, String cmd) {
	this->cmd = String_Clone(cmd);
	Array_Init(this->params, 0);
}

void Process_Destroy(Process *this) {
	String_Destroy(&this->cmd);

	Array_Foreach(this->params, String_Destroy);
	Array_Destroy(this->params);
}

void Process_AddParameter(Process *this, String param) {
	Array_Push(this->params, String_Clone(param));
}

String Process_GetCommandLine(Process *this) {
	String out = HeapString(128);

	String_Append(&out, this->cmd);
	String_Append(&out, ' ');

	for (size_t i = 0; i < this->params->len; i++) {
		String_Append(&out, this->params->buf[i]);
		String_Append(&out, ' ');
	}

	return out;
}

int OVERLOAD Process_Spawn(Process *this, float *time) {
	char *argv[this->params->len + 1];

	argv[0] = String_ToNulHeap(&this->cmd);

	size_t i;

	for (i = 0; i < this->params->len; i++) {
		argv[i + 1] = String_ToNulHeap(&this->params->buf[i]);
	}

	argv[i + 1] = NULL;

	pid_t pid, w;
	int ret = -1;
	int status;

	if ((pid = fork()) == 0) { /* child */
		if (execve(argv[0], argv, environ) < 0) {
			for (size_t i = 0; argv[i] != NULL; i++) {
				Memory_Free(argv[i]);
			}

			throw(exc, &Process_SpawningProcessFailedException);
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
			Memory_Free(argv[i]);
		}

		throw(exc, &Process_ForkFailedException);
	}

	for (size_t i = 0; argv[i] != NULL; i++) {
		Memory_Free(argv[i]);
	}

	return ret;
}

int OVERLOAD Process_Spawn(Process *this) {
	return Process_Spawn(this, NULL);
}
