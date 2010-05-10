#ifndef HTTP_H
#define HTTP_H

typedef void *(* HTTP_OnParameter)(void *, String);

typedef enum {
	HTTP_Version_1_0,
	HTTP_Version_1_1
} HTTP_Version;

typedef struct {
	String key;
	String value;
} HTTP_Header;

#endif
