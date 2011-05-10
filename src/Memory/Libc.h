#import <stdlib.h>

#import "../Memory.h"

#define self Memory_Libc

class {

};

rsdef(self, New);
def(void, Destroy);
def(Memory_Chunk *, Allocate, size_t size);
def(void, Release, Memory_Chunk *chunk);
def(Memory_Chunk *, Resize, Memory_Chunk *chunk, size_t size);

ExportImpl(Memory);

#undef self
