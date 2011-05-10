#import "Logger.h"

#define self Memory_Logger

rsdef(self, New, Memory parent) {
	return (self) {
		.parent = parent,
	};
}

def(void, Destroy) { }

def(Memory_Chunk *, Allocate, size_t size) {
	Memory_Chunk *chunk = delegate(this->parent, allocate, size);
	printf("Allocated %i (%i) bytes (@%p).\n", chunk->size, size, chunk->data);
	return chunk;
}

def(void, Release, Memory_Chunk *chunk) {
	size_t oldSize = chunk->size;
	void  *oldData = chunk->data;

	delegate(this->parent, release, chunk);

	printf("Released %i bytes (@%p).\n", oldSize, oldData);
}

def(Memory_Chunk *, Resize, Memory_Chunk *chunk, size_t size) {
	size_t oldSize = chunk->size;
	void  *oldData = chunk->data;

	chunk = delegate(this->parent, resize, chunk, size);

	printf("Resized %i bytes to %i (%i) bytes (@%p => @%p).\n",
		oldSize, chunk->size, size, oldData, chunk->data);

	return chunk;
}

Impl(Memory) = {
	.allocate = ref(Allocate),
	.release  = ref(Release),
	.resize   = ref(Resize)
};
