#import "File.h"
#import "StreamInterface.h"

#define self FileStream

#define FileStreamInstance FileInstance

def(void, Open, RdString path, int mode);
def(void, Close);
def(size_t, Read, void *buf, size_t len);
def(size_t, Write, void *buf, size_t len);
def(bool, IsEof);

ExportImpl(Stream);

static inline Stream File_AsStream(File *file) {
	return FileStream_AsStream(file);
}

#undef self
