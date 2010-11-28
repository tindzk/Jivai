#import "File.h"
#import "StreamInterface.h"

#define FileStreamInstance FileInstance

#undef self
#define self FileStream

def(void, Open, String path, int mode);
def(void, Close);
def(size_t, Read, void *buf, size_t len);
def(size_t, Write, void *buf, size_t len);
def(bool, IsEof);

extern Impl(Stream);
