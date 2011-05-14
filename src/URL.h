#import "String.h"
#import "Integer.h"

#define self URL

record(ref(Parts)) {
	String scheme;
	String host;
	short port;
	String path;
	String fragment;
};

sdef(ref(Parts), Parse, RdString url);
sdef(void, Parts_Destroy, ref(Parts) *parts);

#undef self
