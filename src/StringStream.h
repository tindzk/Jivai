#import "String.h"
#import "StreamInterface.h"

typedef struct {
	String *str;
	size_t offset;
} StringStream;

void StringStream_Init(StringStream *this, String *s);
size_t StringStream_Read(StringStream *this, void *buf, size_t len);
size_t StringStream_Write(StringStream *this, void *buf, size_t len);
void StringStream_Close(__unused StringStream *this);
bool StringStream_IsEof(StringStream *this);

extern StreamInterface StringStreamImpl;
