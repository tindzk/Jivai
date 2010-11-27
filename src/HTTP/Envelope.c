#import "Envelope.h"

def(void, Init) {
	this->contentLength = -1;
	this->location      = HeapString(0);
	this->contentType   = HeapString(0);
	this->cookieName    = HeapString(0);
	this->cookieValue   = HeapString(0);
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
	String_Copy(&this->location, url);
}

def(void, SetContentType, String contentType) {
	String_Copy(&this->contentType, contentType);
}

def(void, SetCookie, String name, String value) {
	String_Copy(&this->cookieName,  name);
	String_Copy(&this->cookieValue, value);
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
	String s = HeapString(1024);

	HTTP_Status_Item st = HTTP_Status_GetItem(this->status);

	String_FmtAppend(&s, $("HTTP/% % %\r\n"),
		(this->version == HTTP_Version_1_1)
			? $("1.1")
			: $("1.0"),
		Integer_ToString(st.code),
		st.msg);

	if (this->contentLength != -1) {
		String_FmtAppend(&s, $("Content-Length: %\r\n"),
			Integer_ToString(this->contentLength));
	}

	if (this->contentType.len != 0) {
		String_FmtAppend(&s, $("Content-Type: %\r\n"),
			this->contentType);
	}

	if (this->lastModified.date.day != 0) {
		String tmp = Date_RFC822_ToString(this->lastModified);
		String_FmtAppend(&s, $("Last-Modified: %\r\n"), tmp);
		String_Destroy(&tmp);
	}

	if (this->location.len > 0) {
		String_FmtAppend(&s, $("Location: %\r\n"), this->location);
	}

	if (this->cookieName.len > 0) {
		String_FmtAppend(&s, $("Set-Cookie: %=%; path=/\r\n"),
			this->cookieName,
			this->cookieValue);
	}

	String_Append(&s, this->persistent
		? $("Connection: Keep-Alive\r\n")
		: $("Connection: Close\r\n"));

	String_Append(&s, $("\r\n"));

	return s;
}
