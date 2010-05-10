#include "FileStream.h"

StreamInterface FileStream_Methods = {
	.read  = (void *) FileStream_Read,
	.write = (void *) FileStream_Write,
	.close = (void *) FileStream_Close
};
