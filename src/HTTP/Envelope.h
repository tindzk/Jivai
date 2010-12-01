#import "../String.h"
#import "../DateTime.h"
#import "../Date/RFC822.h"

#import "Status.h"
#import "Version.h"

#define self HTTP_Envelope

class {
	s64          contentLength;
	String       location;
	String       contentType;
	String       cookieName;
	String       cookieValue;
	HTTP_Version version;
	HTTP_Status  status;
	bool         persistent;
	Date_RFC822  lastModified;
};

def(void, Init);
def(void, Destroy);
def(void, SetContentLength, s64 len);
def(s64, GetContentLength);
def(void, SetLocation, String url);
def(void, SetContentType, String contentType);
def(void, SetCookie, String name, String value);
def(void, SetVersion, HTTP_Version version);
def(void, SetStatus, HTTP_Status status);
def(void, SetPersistent, bool value);
def(bool, IsPersistent);
def(void, SetLastModified, DateTime lastModified);
def(String, GetString);

#undef self
