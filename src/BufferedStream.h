#import "String.h"
#import "Memory.h"
#import "Stream.h"

#define self BufferedStream

class {
	Stream stream;

	bool eof;

	String inbuf;
	size_t inbufThreshold;

	String outbuf;
};

rsdef(self, New, Stream stream);
def(void, Destroy);
def(void, SetInputBuffer, size_t size, size_t threshold);
def(void, SetOutputBuffer, size_t size);
def(bool, IsEof);
def(size_t, Read, void *buf, size_t len);
def(size_t, Write, void *buf, size_t len);
def(RdString, Flush);
def(void, Reset);
def(void, Close);

ExportImpl(Stream);

#undef self
