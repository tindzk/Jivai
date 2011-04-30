#import "File.h"
#import "Stream.h"

#define self FileStream

#define FileStreamInst    FileInst
#define FileStreamDynInst FileDynInst

def(void, Close);
def(size_t, Read, void *buf, size_t len);
def(size_t, Write, void *buf, size_t len);
def(bool, IsEof);

ExportImpl(Stream);

static inline Stream File_AsStream(File *file) {
	return FileStream_AsStream(file);
}

#undef self
