#import "Envelope.h"

#define self HTTP_Envelope

def(void, Init) {
	this->contentLength = -1;
	this->location      = String_New(0);
	this->contentType   = String_New(0);
	this->cookieName    = String_New(0);
	this->cookieValue   = String_New(0);
	this->version       = HTTP_Version_1_1;
	this->status        = HTTP_Status_Success_Ok;
	this->persistent    = false;
	this->lastModified  = Date_RFC822_Empty();
}

def(void, Destroy) {
	String_Destroy(&this->contentType);
	String_Destroy(&this->location);
	String_Destroy(&this->cookieName);
	String_Destroy(&this->cookieValue);
}

def(void, SetContentLength, s64 len) {
	this->contentLength = len;
}

def(s64, GetContentLength) {
	return this->contentLength;
}

def(void, SetLocation, String url) {
	String_Assign(&this->location, &url);
}

def(void, SetContentType, String contentType) {
	String_Assign(&this->contentType, &contentType);
}

def(void, SetCookie, String name, String value) {
	String_Assign(&this->cookieName,  &name);
	String_Assign(&this->cookieValue, &value);
}

def(void, SetVersion, HTTP_Version version) {
	this->version = version;
}

def(void, SetStatus, HTTP_Status status) {
	this->status = status;
}

def(void, SetPersistent, bool value) {
	this->persistent = value;
}

def(bool, IsPersistent) {
	return this->persistent;
}

def(void, SetLastModified, DateTime lastModified) {
	this->lastModified = Date_RFC822_FromDateTime(lastModified);
}

def(String, GetString) {
	String s = String_New(1024);

	HTTP_Status_Item st = HTTP_Status_GetItem(this->status);

	String strCode = Integer_ToString(st.code);
	String_Append(&s, FmtString($("% % %\r\n"),
		HTTP_Version_ToString(this->version),
		strCode.prot, st.msg));
	String_Destroy(&strCode);

	if (this->contentLength != -1) {
		String strValue = Integer_ToString(this->contentLength);
		String_Append(&s, FmtString($("Content-Length: %\r\n"),
			strValue.prot));
		String_Destroy(&strValue);
	}

	if (this->contentType.len != 0) {
		String_Append(&s, FmtString($("Content-Type: %\r\n"),
			this->contentType.prot));
	}

	if (this->lastModified.date.day != 0) {
		String tmp = Date_RFC822_ToString(this->lastModified);
		String_Append(&s, FmtString($("Last-Modified: %\r\n"), tmp.prot));
		String_Destroy(&tmp);
	}

	if (this->location.len > 0) {
		String_Append(&s, FmtString($("Location: %\r\n"),
			this->location.prot));
	}

	if (this->cookieName.len > 0) {
		String_Append(&s, FmtString(
			$("Set-Cookie: %=%; path=/\r\n"),
			this->cookieName.prot,
			this->cookieValue.prot));
	}

	String_Append(&s, this->persistent
		? $("Connection: Keep-Alive\r\n")
		: $("Connection: Close\r\n"));

	String_Append(&s, $("\r\n"));

	return s;
}
