#import "../Hex.h"
#import "../String.h"
#import "../Integer.h"
#import "../Unicode.h"

#ifndef HTML_Entities_GrowthFactor
#define HTML_Entities_GrowthFactor 1.3
#endif

#define self HTML_Entities

sdef(String, Decode, String s);
overload sdef(void, Encode, String s, String *out);
overload sdef(String, Encode, String s);

#undef self
