#import "Hex.h"
#import "File.h"
#import "String.h"
#import "Memory.h"
#import "Integer.h"
#import "Compiler.h"

#ifdef Backtrace_HasBFD
#import "BFD.h"
#endif

void Backtrace_PrintTrace(void **dest, size_t size);
void* Backtrace_GetFrameAddr(u32 level);
void* Backtrace_GetReturnAddr(u32 level);
size_t Backtrace_GetTrace(void **buf, u32 size);
