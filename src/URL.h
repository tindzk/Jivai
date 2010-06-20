#ifndef URL_H
#define URL_H

#include "String.h"

typedef struct {
	String scheme;
	String host;
	short port;
	String path;
} URL_Parts;

typedef enum {
	URL_State_Scheme,
	URL_State_Host,
	URL_State_Port,
	URL_State_Path
} URL_State;

URL_Parts URL_Parse(String url);

#endif
