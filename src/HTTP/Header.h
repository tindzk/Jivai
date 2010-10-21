#import "Query.h"
#import "Method.h"
#import "Version.h"

#import "../HTTP.h"
#import "../Char.h"
#import "../String.h"
#import "../Integer.h"
#import "../Exception.h"

#undef self
#define self HTTP_Header

enum {
	excEmptyRequestUri = excOffset,
	excRequestMalformed,
	excUnknownMethod,
	excUnknownStatus,
	excUnknownVersion
};

typedef enum {
	HTTP_Header_Type_Request,
	HTTP_Header_Type_Response
} HTTP_Header_Type;

typedef struct {
	HTTP_OnParameter onParameter;
	HTTP_OnVersion   onVersion;
	HTTP_OnMethod    onMethod;
	HTTP_OnHeader    onHeader;
	HTTP_OnStatus    onStatus;
	HTTP_OnPath      onPath;
	void             *context;
} HTTP_Header_Events;

typedef struct {
	HTTP_Header_Events events;
} HTTP_Header;

void HTTP_Header0(ExceptionManager *e);
void HTTP_Header_Init(HTTP_Header *this, HTTP_Header_Events events);
void HTTP_Header_ParseMethod(HTTP_Header *this, String s);
void HTTP_Header_ParseVersion(HTTP_Header *this, String s);
void HTTP_Header_ParseStatus(HTTP_Header *this, String s);
void HTTP_Header_ParseUri(HTTP_Header *this, String s);
void HTTP_Header_ParseHeaderLine(HTTP_Header *this, String s);
ssize_t HTTP_Header_GetLength(String str);
void HTTP_Header_Parse(HTTP_Header *this, HTTP_Header_Type type, String s);
