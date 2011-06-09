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

sdef(void, PrintTrace, void **addr, uint size);
sdef(void *, GetFrameAddr, uint level);
sdef(void *, GetReturnAddr, uint level);
sdef(uint, GetTrace, void **buf, uint size);

#undef self
