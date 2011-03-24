#import "Query.h"
#import "Header.h"
#import "Status.h"
#import "Method.h"
#import "ContentType.h"

#import "../HTTP.h"
#import "../String.h"
#import "../Integer.h"
#import "../Exception.h"
#import "../SocketConnection.h"

#define self HTTP_Server

// @exc BodyTooLarge
// @exc BodyUnexpected
// @exc HeaderTooLarge
// @exc UnknownContentType

Callback(ref(OnRespond), void, bool);

set(ref(State)) {
	ref(State_Header),
	ref(State_Body),
	ref(State_Dispatch)
};

set(ref(Result)) {
	ref(Result_Incomplete),
	ref(Result_Complete),
	ref(Result_Error)
};

class {
	bool               cleanup;
	String             body;
	String             header;
	size_t             maxHeaderLength;
	u64                maxBodyLength;
	HTTP_Method        method;
	SocketConnection   *conn;
	ref(State)         state;

	struct {
		HTTP_OnMethod    onMethod;
		HTTP_OnVersion   onVersion;
		HTTP_OnPath      onPath;
		HTTP_OnHeader    onHeader;
		HTTP_OnParameter onBodyParameter;
		HTTP_OnParameter onQueryParameter;
		ref(OnRespond)   onRespond;
	} events;

	struct {
		HTTP_ContentType contentType;
		u64              contentLength;
		bool             persistentConnection;
		String           boundary;
	} headers;
};

rsdef(self, New, SocketConnection *conn, size_t maxHeaderLength, u64 maxBodyLength);
def(void, Destroy);
def(void, BindMethod, HTTP_OnMethod onMethod);
def(void, BindVersion, HTTP_OnVersion onVersion);
def(void, BindPath, HTTP_OnPath onPath);
def(void, BindHeader, HTTP_OnHeader onHeader);
def(void, BindBodyParameter, HTTP_OnParameter onBodyParameter);
def(void, BindQueryParameter, HTTP_OnParameter onQueryParameter);
def(void, BindRespond, ref(OnRespond) onRespond);
def(ref(Result), ReadHeader);
def(ref(Result), ReadBody);
def(bool, Process);

#undef self
