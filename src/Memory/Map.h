#define _GNU_SOURCE /* for mremap() */

#import <sys/mman.h>

#import "../Memory.h"

#define self Memory_Map

class {

};

rsdef(self, New);
def(void, Destroy);
def(Memory_Chunk *, Allocate, size_t size);
def(void, Release, Memory_Chunk *chunk);
def(Memory_Chunk *, Resize, Memory_Chunk *chunk, size_t size);

ExportImpl(Memory);

#undef self
