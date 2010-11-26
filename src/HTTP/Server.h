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

#undef self
#define self HTTP_Server

enum {
	excBodyTooLarge = excOffset,
	excBodyUnexpected,
	excHeaderTooLarge,
	excUnknownContentType
};

DefineCallback(ref(OnRespond), void, bool);

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

record(ref(Events)) {
	HTTP_OnMethod    onMethod;
	HTTP_OnVersion   onVersion;
	HTTP_OnPath      onPath;
	HTTP_OnHeader    onHeader;
	HTTP_OnParameter onBodyParameter;
	HTTP_OnParameter onQueryParameter;
	ref(OnRespond)   onRespond;
};

class(self) {
	bool               cleanup;
	String             body;
	String             header;
	size_t             maxHeaderLength;
	u64                maxBodyLength;
	HTTP_Method        method;
	SocketConnection   *conn;
	ref(State)         state;
	ref(Events)        events;

	struct {
		HTTP_ContentType contentType;
		u64              contentLength;
		bool             persistentConnection;
		String           boundary;
	} headers;
};

def(void, Init, ref(Events) events, SocketConnection *conn, size_t maxHeaderLength, u64 maxBodyLength);
def(void, Destroy);
def(void, OnMethod, HTTP_Method method);
def(void, OnVersion, HTTP_Version version);
def(void, OnPath, String path);
def(String *, OnQueryParameter, String name);
def(void, OnHeader, String name, String value);
def(ref(Result), ReadHeader);
def(ref(Result), ReadBody);
def(bool, Process);
