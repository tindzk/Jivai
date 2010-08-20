#import <stddef.h>

typedef struct {
	size_t (*read)(void *, void *, size_t);
	size_t (*write)(void *, void *, size_t);
	void (*close)(void *);
	bool (*isEof)(void *);
} StreamInterface;
