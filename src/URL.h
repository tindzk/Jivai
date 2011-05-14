#import "String.h"
#import "Integer.h"

#define self URL

// @exc SchemeMissing

record(ref(Parts)) {
	RdString scheme;
	RdString host;
	unsigned short port;
	RdString path;
	RdString fragment;
};

sdef(ref(Parts), Parse, RdString url);

#undef self
