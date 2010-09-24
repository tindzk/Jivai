#import "Hex.h"
#import "String.h"
#import "Memory.h"
#import "Integer.h"
#import "Compiler.h"

#ifdef Backtrace_HasBFD
#import "BFD.h"
#endif

void Backtrace_PrintTrace(void **dest, size_t size);
void* Backtrace_GetFrameAddr(int level);
void* Backtrace_GetReturnAddr(int level);
size_t Backtrace_GetTrace(void **buf, size_t size);
