#import "String.h"
#import "Memory.h"
#import "StreamInterface.h"

#undef self
#define self BufferedStream

class {
	StreamInterface *stream;
	void *data;

	bool eof;

	String inbuf;
	size_t inbufThreshold;

	String outbuf;
};

def(void, Init, StreamInterface *stream, void *data);
def(void, Destroy);
def(void, SetInputBuffer, size_t size, size_t threshold);
def(void, SetOutputBuffer, size_t size);
def(bool, IsEof);
def(size_t, Read, void *buf, size_t len);
def(size_t, Write, void *buf, size_t len);
def(String, Flush);
def(void, Reset);
def(void, Close);

StreamInterface Impl(self);
