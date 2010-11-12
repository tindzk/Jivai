#import "String.h"
#import "Integer.h"

#undef self
#define self URL

record(ref(Parts)) {
	String scheme;
	String host;
	short port;
	String path;
	String fragment;
};

sdef(ref(Parts), Parse, String url);
sdef(void, Parts_Destroy, ref(Parts) *parts);
