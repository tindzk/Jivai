#import "../Hex.h"
#import "../HTTP.h"
#import "../String.h"

#undef self
#define self HTTP_Query

enum {
	excExceedsPermittedLength = excOffset
};

typedef struct {
	HTTP_OnParameter onParameter;
	void *context;
	bool autoResize;
} HTTP_Query;

void HTTP_Query0(ExceptionManager *e);

void HTTP_Query_Init(HTTP_Query *this, HTTP_OnParameter onParameter, void *context);
void HTTP_Query_SetAutoResize(HTTP_Query *this, bool value);
size_t HTTP_Query_GetAbsoluteLength(String s);
void HTTP_Query_Unescape(String src, char *dst, bool isFormUri);
void HTTP_Query_Decode(HTTP_Query *this, String s, bool isFormUri);
