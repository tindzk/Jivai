#import "Array.h"

typedef struct {
	size_t len;
	size_t size;
	String buf[0];
} StringArray;
