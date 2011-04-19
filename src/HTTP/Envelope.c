#import "Envelope.h"

#define self HTTP_Envelope

rsdef(self, New) {
	return (self) {
		.contentLength = -1,
		.location      = CarrierString_New(),
		.contentType   = CarrierString_New(),
		.cookieName    = CarrierString_New(),
		.cookieValue   = CarrierString_New(),
		.version       = HTTP_Version_1_1,
		.status        = HTTP_Status_Success_Ok,
		.persistent    = false,
		.lastModified  = Date_RFC822_New()
	};
}

def(void, Destroy) {
	CarrierString_Destroy(&this->contentType);
	CarrierString_Destroy(&this->location);
	CarrierString_Destroy(&this->cookieName);
	CarrierString_Destroy(&this->cookieValue);
}

def(void, SetContentLength, s64 len) {
	this->contentLength = len;
}

def(s64, GetContentLength) {
	return this->contentLength;
}

def(void, SetLocation, CarrierString url) {
	CarrierString_Assign(&this->location, url);
}

def(void, SetContentType, CarrierString contentType) {
	CarrierString_Assign(&this->contentType, contentType);
}

def(void, SetCookie, CarrierString name, CarrierString value) {
	CarrierString_Assign(&this->cookieName,  name);
	CarrierString_Assign(&this->cookieValue, value);
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
		strCode.rd, st.msg));
	String_Destroy(&strCode);

	if (this->contentLength != -1) {
		String strValue = Integer_ToString(this->contentLength);
		String_Append(&s, FmtString($("Content-Length: %\r\n"),
			strValue.rd));
		String_Destroy(&strValue);
	}

	if (this->contentType.len != 0) {
		String_Append(&s, FmtString($("Content-Type: %\r\n"),
			this->contentType.rd));
	}

	if (this->lastModified.date.day != 0) {
		String tmp = Date_RFC822_ToString(this->lastModified);
		String_Append(&s, FmtString($("Last-Modified: %\r\n"), tmp.rd));
		String_Destroy(&tmp);
	}

	if (this->location.len > 0) {
		String_Append(&s, FmtString($("Location: %\r\n"),
			this->location.rd));
	}

	if (this->cookieName.len > 0) {
		String_Append(&s, FmtString(
			$("Set-Cookie: %=%; path=/\r\n"),
			this->cookieName.rd,
			this->cookieValue.rd));
	}

	String_Append(&s, this->persistent
		? $("Connection: Keep-Alive\r\n")
		: $("Connection: Close\r\n"));

	String_Append(&s, $("\r\n"));

	return s;
}
