#import "String.h"
#import "Exception.h"

#ifndef Module_Approximation
#define Module_Approximation 100
#endif

typedef struct {
	String name;
} Module;

size_t Module_Register(String name);
String Module_ResolveName(size_t module);
