#import "ELF.h"
#import "Hex.h"
#import "Path.h"
#import "DWARF.h"
#import "String.h"
#import "Memory.h"
#import "Channel.h"
#import "Integer.h"
#import "Compiler.h"

#define self Backtrace

sdef(void, PrintTrace, void **addr, size_t size);
sdef(void *, GetFrameAddr, u32 level);
sdef(void *, GetReturnAddr, u32 level);
sdef(size_t, GetTrace, void **buf, u32 size);

#undef self
