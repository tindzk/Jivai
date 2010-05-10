#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "Query.h"
#include "Status.h"
#include "Method.h"

#include "../Exception.h"
#include "../String.h"
#include "../HTTP.h"
#include "../SocketConnection.h"

Exception_Export(HTTP_Request_BodyTooLargeException);
Exception_Export(HTTP_Request_BodyUnexpectedException);
Exception_Export(HTTP_Request_EmptyRequestUriException);
Exception_Export(HTTP_Request_HeaderTooLargeException);
Exception_Export(HTTP_Request_RequestMalformedException);
Exception_Export(HTTP_Request_UnknownContentTypeException);
Exception_Export(HTTP_Request_UnknownMethodException);
Exception_Export(HTTP_Request_UnknownVersionException);

typedef void (* HTTP_Request_OnHttpVersion)(void *, HTTP_Version);
typedef void (* HTTP_Request_OnMethod)(void *, HTTP_Method);
typedef void (* HTTP_Request_OnPath)(void *, String);
typedef void (* HTTP_Request_OnHeader)(void *, String, String);
typedef bool (* HTTP_Request_OnRespond)(void *, bool);

typedef enum {
	HTTP_Request_ContentType_Unset,
	HTTP_Request_ContentType_SinglePart,
	HTTP_Request_ContentType_MultiPart
} HTTP_Request_ContentType;

typedef struct {
	HTTP_Request_ContentType contentType;
	uint64_t contentLength;
	bool persistentConnection;
	String boundary;
} HTTP_Request_Headers;

typedef enum {
	HTTP_Request_State_Header,
	HTTP_Request_State_Body,
	HTTP_Request_State_Dispatch
} HTTP_Request_State;

typedef enum {
	HTTP_Request_Result_Incomplete,
	HTTP_Request_Result_Complete,
	HTTP_Request_Result_Error
} HTTP_Request_Result;

typedef struct {
	void *context;

	size_t maxHeaderLength;
	uint64_t maxBodyLength;

	bool clean;

	HTTP_Method method;
	HTTP_Request_Headers headers;

	HTTP_Request_State state;

	String header;
	String body;

	HTTP_Request_OnHttpVersion onHttpVersion;
	HTTP_Request_OnMethod onMethod;
	HTTP_Request_OnPath onPath;
	HTTP_Request_OnHeader onHeader;
	HTTP_Request_OnRespond onRespond;
	HTTP_OnParameter onQueryParameter;
	HTTP_OnParameter onBodyParameter;
} HTTP_Request;

void HTTP_Request0(ExceptionManager *e);

void HTTP_Request_Init(HTTP_Request *this, size_t maxHeaderLength, uint64_t maxBodyLength);
void HTTP_Request_Destroy(HTTP_Request *this);
void HTTP_Request_ParseHttpVersion(HTTP_Request *this, String version);
void HTTP_Request_ParseMethod(HTTP_Request *this, String method);
void HTTP_Request_ParseUri(HTTP_Request *this, String uri);
void HTTP_Request_ParseHeaderLine(HTTP_Request *this, String s);
HTTP_Request_Result HTTP_Request_ReadHeader(HTTP_Request *this, SocketConnection *conn);
HTTP_Request_Result HTTP_Request_ReadBody(HTTP_Request *this, SocketConnection *conn);
HTTP_Request_Result HTTP_Request_Dispatch(HTTP_Request *this);
bool HTTP_Request_Parse(HTTP_Request *this, SocketConnection *conn);

#endif
