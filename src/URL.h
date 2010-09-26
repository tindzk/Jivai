#import "String.h"

typedef struct {
	String scheme;
	String host;
	short port;
	String path;
	String fragment;
} URL_Parts;

typedef enum {
	URL_State_Scheme,
	URL_State_Host,
	URL_State_Port,
	URL_State_Path,
	URL_State_Fragment
} URL_State;

URL_Parts URL_Parse(String url);
void URL_Parts_Destroy(URL_Parts *this);
