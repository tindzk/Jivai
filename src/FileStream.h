#import "File.h"
#import "StreamInterface.h"

#define FileStream       File
#define FileStream_Open  File_Open
#define FileStream_Close File_Close
#define FileStream_Read  File_Read

size_t FileStream_Write(FileStream *this, void *buf, size_t len);
bool FileStream_IsEof(__unused FileStream *this);

extern StreamInterface FileStream_Methods;
