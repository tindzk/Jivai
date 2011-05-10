#import "Libc.h"

#define self Memory_Libc

rsdef(self, New) {
	return (self) { };
}

def(void, Destroy) { }

def(Memory_Chunk *, Allocate, size_t size) {
	Memory_Chunk *chunk = malloc(sizeof(Memory_Chunk) + size);

	if (chunk == NULL) {
		return NULL;
	}

	chunk->size = size;

	return chunk;
}

def(void, Release, Memory_Chunk *chunk) {
	free(chunk);
}

def(Memory_Chunk *, Resize, Memory_Chunk *chunk, size_t size) {
	chunk = realloc(chunk, sizeof(Memory_Chunk) + size);

	if (chunk == NULL) {
		return NULL;
	}

	chunk->size = size;

	return chunk;
}

Impl(Memory) = {
	.allocate = ref(Allocate),
	.release  = ref(Release),
	.resize   = ref(Resize)
};
