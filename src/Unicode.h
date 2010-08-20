#import "String.h"
#import "Charset.h"

size_t Unicode_CalcWidth(const char *src);
size_t Unicode_Next(String s, size_t offset);
size_t Unicode_Prev(String s, size_t offset);
size_t OVERLOAD Unicode_Count(String s, size_t offset, size_t len);
size_t OVERLOAD Unicode_Count(String s);
void Unicode_ToMultiByte(int c, String *res);
