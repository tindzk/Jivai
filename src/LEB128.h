#import "Types.h"
#import "Exception.h"

#define self LEB128

rsdef(size_t, ReadSigned, ubyte *addr, s32 *ret);
rsdef(size_t, ReadUnsigned, ubyte *addr, u32 *ret);

#undef self
