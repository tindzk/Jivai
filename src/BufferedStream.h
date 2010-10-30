#import "String.h"
#import "Memory.h"
#import "StreamInterface.h"

typedef struct {
	StreamInterface *stream;
	void *data;

	bool eof;

	String inbuf;
	size_t inbufThreshold;

	String outbuf;
} BufferedStream;

void BufferedStream_Init(BufferedStream *this, StreamInterface *stream, void *data);
void BufferedStream_Destroy(BufferedStream *this);
void BufferedStream_SetInputBuffer(BufferedStream *this, size_t size, size_t threshold);
void BufferedStream_SetOutputBuffer(BufferedStream *this, size_t size);
bool BufferedStream_IsEof(BufferedStream *this);
size_t BufferedStream_Read(BufferedStream *this, void *buf, size_t len);
size_t BufferedStream_Write(BufferedStream *this, void *buf, size_t len);
String BufferedStream_Flush(BufferedStream *this);
void BufferedStream_Reset(BufferedStream *this);
void BufferedStream_Close(BufferedStream *this);

extern StreamInterface BufferedStreamImpl;
