#import "../String.h"
#import "../DateTime.h"
#import "../Date/RFC822.h"

#import "Status.h"
#import "Version.h"

#define self HTTP_Envelope

class {
	s64           contentLength;
	CarrierString location;
	CarrierString contentType;
	CarrierString cookieName;
	CarrierString cookieValue;
	HTTP_Version  version;
	HTTP_Status   status;
	bool          persistent;
	Date_RFC822   lastModified;
};

rsdef(self, New);
def(void, Destroy);
def(void, SetContentLength, s64 len);
def(s64, GetContentLength);
def(void, SetLocation, CarrierString url);
def(void, SetContentType, CarrierString contentType);
def(void, SetCookie, CarrierString name, CarrierString value);
def(void, SetVersion, HTTP_Version version);
def(void, SetStatus, HTTP_Status status);
def(void, SetPersistent, bool value);
def(bool, IsPersistent);
def(void, SetLastModified, DateTime lastModified);
def(String, GetString);

#undef self
