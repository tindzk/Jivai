#import "../Hex.h"
#import "../HTTP.h"
#import "../String.h"

Exception_Export(HTTP_Query_ExceedsPermittedLengthException);

typedef struct {
	HTTP_OnParameter onParameter;
	void *context;
} HTTP_Query;

void HTTP_Query0(ExceptionManager *e);

void HTTP_Query_Init(HTTP_Query *this, HTTP_OnParameter onParameter, void *context);
size_t HTTP_Query_GetAbsoluteLength(String s);
void HTTP_Query_Unescape(String src, char *dst, bool isFormUri);
void HTTP_Query_Decode(HTTP_Query *this, String s, bool isFormUri);
