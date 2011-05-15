#import "String.h"
#import "Exception.h"

#define self StringReader

class {
	size_t ofs;
	size_t line;
	RdString buf;
};

rsdef(self, New, RdString s);
def(bool, IsEnd);
overload def(bool, Peek, char *c);
overload def(bool, Peek, char *c, size_t cnt);
overload def(bool, Peek, RdString *str, size_t len);
overload def(void, Consume);
overload def(void, Consume, size_t len);
def(void, Extend, RdString *str);

static alwaysInline rdef(size_t, GetLine) {
	return this->line;
}

#undef self
