#import "ELF.h"
#import "Path.h"
#import "File.h"
#import "String.h"
#import "HashTable.h"

#define self Locale

#ifndef Namespace
	#error No namespace set.
#endif

exc(CorruptFile)
exc(Duplicate)

record(ref(Item)) {
	String dest;
};

record(ref(Context)) {
	String path;
	bool loaded;
};

Callback(ref(OnMessage), void, RdString msg);

set(ref(CheckError)) {
	ref(CheckError_Invalid),
	ref(CheckError_Missing),
	ref(CheckError_Empty)
};

Callback(ref(OnCheckError), void, ref(CheckError) type, RdString msg);

class {
	HashTable tbl;
	HashTable contexts;
	String language;
};

rsdef(self, New);
def(void, Destroy);
sdef(String, decode, RdString s);
sdef(String, encode, RdString s);
sdef(void, createInitial, RdString binary, RdString out);
sdef(void, check, RdString binary, RdString path, ref(OnCheckError) cb);
sdef(void, dump, RdString path, ref(OnMessage) cb);
overload sdef(void, load, RdString path, HashTable *tbl);
overload def(void, load, RdString path);
def(void, addContext, RdString context, String path);
def(bool, hasContext, RdString context);
def(void, flush);
def(void, setLanguage, RdString lng);
def(RdString, translate, RdString context, RdString value);

#define tc(this, s) \
	Locale_translate(this, $(Namespace), RefString(".locale", s))

#define t(s) \
	tc(Locale_GetInstance(), s)

SingletonPrototype(self);

#undef self
