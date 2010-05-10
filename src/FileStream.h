#ifndef FILE_STREAM_H
#define FILE_STREAM_H

#include "File.h"
#include "StreamInterface.h"

#define FileStream       File
#define FileStream_Open  File_Open
#define FileStream_Close File_Close
#define FileStream_Read  File_Read
#define FileStream_Write File_Write

extern StreamInterface FileStream_Methods;

#endif
