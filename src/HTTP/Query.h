#ifndef HTTP_QUERY_H
#define HTTP_QUERY_H

#include "../String.h"
#include "../HTTP.h"

Exception_Export(HTTP_Query_ExceedsPermittedLengthException);

void HTTP_Query0(ExceptionManager *e);

void HTTP_Query_Decode(String s, bool isFormUri, HTTP_OnParameter onParameter, void *context);

#endif
