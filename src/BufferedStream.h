#import "Buffer.h"
#import "Stream.h"
#import "Exception.h"

#define self BufferedStream

class {
	Stream stream;

	bool eof;

	Buffer in;
	size_t inThreshold;

	Buffer out;
};

rsdef(self, New, Stream stream);
def(void, Destroy);
def(void, SetInputBuffer, size_t size, size_t threshold);
def(void, SetOutputBuffer, size_t size);
def(bool, IsEof);
def(size_t, Read, WrBuffer buf);
def(size_t, Write, RdBuffer buf);
def(RdBuffer, Flush);
def(void, Reset);
def(void, Close);

ExportImpl(Stream);

#undef self
