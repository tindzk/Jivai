#ifndef CONTENT_TYPE_H
#define CONTENT_TYPE_H

typedef enum {
	HTTP_ContentType_Unset,
	HTTP_ContentType_SinglePart,
	HTTP_ContentType_MultiPart
} HTTP_ContentType;

#endif
