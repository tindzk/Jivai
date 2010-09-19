#import "../String.h"
#import "../Unicode.h"

#ifndef HTML_Entities_GrowthFactor
#define HTML_Entities_GrowthFactor 1.3
#endif

String HTML_Entities_Decode(String s);
overload void HTML_Entities_Encode(String s, String *out);
overload String HTML_Entities_Encode(String s);
