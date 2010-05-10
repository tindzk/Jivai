#ifndef BACKTRACE_H
#define BACKTRACE_H

#include "Hex.h"
#include "Memory.h"
#include "Compiler.h"

#ifdef Backtrace_HasBFD
#include "BFD.h"
#endif

void Backtrace_PrintTrace(void **dest, size_t size);
void* Backtrace_GetFrameAddr(int level);
void* Backtrace_GetReturnAddr(int level);
size_t Backtrace_GetTrace(void **buf, size_t size);

#endif
