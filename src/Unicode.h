#import "String.h"
#import "Charset.h"

#define self Unicode

sdef(size_t, CalcWidth, const char *src);
sdef(size_t, Next, RdString s, size_t offset);
sdef(size_t, Prev, RdString s, size_t offset);
overload sdef(size_t, Count, RdString s, size_t offset, size_t len);
overload sdef(size_t, Count, RdString s);
sdef(void, Shrink, String *s, size_t len);
sdef(bool, ToMultiByte, int c, String *res);

#undef self
