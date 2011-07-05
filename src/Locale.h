#import "ELF.h"
#import "File.h"
#import "String.h"
#import "HashTable.h"

#define self Locale

exc(CorruptFile)
exc(Duplicate)

record(ref(Item)) {
	String dest;
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
};

rsdef(self, New);
def(void, Destroy);
sdef(String, decode, RdString s);
sdef(String, encode, RdString s);
sdef(void, createInitial, RdString binary, RdString path);
sdef(void, check, RdString binary, RdString path, ref(OnCheckError) cb);
sdef(void, dump, RdString path, ref(OnMessage) cb);
overload sdef(void, load, RdString path, HashTable *tbl);
overload def(void, load, RdString path);
def(RdString, getTranslation, RdString value);

#define tc(context, s) \
	Locale_getTranslation(context, RefString(".locale", s))

#define t(s) \
	tc(Locale_GetInstance(), s)

SingletonPrototype(self);

#undef self
