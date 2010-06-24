#ifndef PROCESS_H
#define PROCESS_H

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "String.h"
#include "StringArray.h"
#include "ExceptionManager.h"

Exception_Export(Process_ForkFailedException);
Exception_Export(Process_SpawningProcessFailedException);

extern char **environ;

typedef struct {
	String cmd;
	StringArray params;
} Process;

void Process_Init(Process *this, String cmd);
void Process_Destroy(Process *this);
void Process_AddParameter(Process *this, String param);
String Process_GetCommandLine(Process *this);
int OVERLOAD Process_Spawn(Process *this, float *time);
int OVERLOAD Process_Spawn(Process *this);

#endif
