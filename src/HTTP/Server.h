#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "Query.h"
#include "Header.h"
#include "Status.h"
#include "Method.h"
#include "ContentType.h"

#include "../HTTP.h"
#include "../String.h"
#include "../Exception.h"
#include "../SocketConnection.h"

Exception_Export(HTTP_Server_BodyTooLargeException);
Exception_Export(HTTP_Server_BodyUnexpectedException);
Exception_Export(HTTP_Server_HeaderTooLargeException);
Exception_Export(HTTP_Server_UnknownContentTypeException);

typedef void (* HTTP_Server_OnRespond)(void *, bool);

typedef enum {
	HTTP_Server_State_Header,
	HTTP_Server_State_Body,
	HTTP_Server_State_Dispatch
} HTTP_Server_State;

typedef enum {
	HTTP_Server_Result_Incomplete,
	HTTP_Server_Result_Complete,
	HTTP_Server_Result_Error
} HTTP_Server_Result;

typedef struct {
	HTTP_OnMethod          onMethod;
	HTTP_OnVersion         onVersion;
	HTTP_OnPath            onPath;
	HTTP_OnHeader          onHeader;
	HTTP_OnParameter       onBodyParameter;
	HTTP_OnParameter       onQueryParameter;
	HTTP_Server_OnRespond  onRespond;
	void                   *context;
} HTTP_Server_Events;

typedef struct {
	bool               clean;
	String             body;
	String             header;
	size_t             maxHeaderLength;
	uint64_t           maxBodyLength;
	HTTP_Method        method;
	SocketConnection   *conn;
	HTTP_Server_State  state;
	HTTP_Server_Events events;

	struct {
		HTTP_ContentType contentType;
		uint64_t contentLength;
		bool persistentConnection;
		String boundary;
	} headers;
} HTTP_Server;

void HTTP_Server0(ExceptionManager *e);
void HTTP_Server_Init(HTTP_Server *this, HTTP_Server_Events events, SocketConnection *conn, size_t maxHeaderLength, uint64_t maxBodyLength);
void HTTP_Server_Destroy(HTTP_Server *this);
void HTTP_Server_OnMethod(HTTP_Server *this, HTTP_Method method);
void HTTP_Server_OnVersion(HTTP_Server *this, HTTP_Version version);
void HTTP_Server_OnPath(HTTP_Server *this, String path);
String* HTTP_Server_OnQueryParameter(HTTP_Server *this, String name);
void HTTP_Server_OnHeader(HTTP_Server *this, String name, String value);
HTTP_Server_Result HTTP_Server_ReadHeader(HTTP_Server *this);
HTTP_Server_Result HTTP_Server_ReadBody(HTTP_Server *this);
bool HTTP_Server_Process(HTTP_Server *this);

#endif
