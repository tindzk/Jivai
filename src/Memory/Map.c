#import "Map.h"

#define self Memory_Map

rsdef(self, New) {
	return (self) { };
}

def(void, Destroy) { }

alwaysInline sdef(size_t, AlignSize, size_t size, size_t to) {
	return (size + to - 1) & -to;
}

def(Memory_Chunk *, Allocate, size_t size) {
	size = scall(AlignSize, size + sizeof(Memory_Chunk), Memory_PageSize);

	Memory_Chunk *chunk = mmap(NULL, size,
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS | MAP_UNINITIALIZED, -1, 0);

	if (chunk == MAP_FAILED) {
		return NULL;
	}

	chunk->size = size - sizeof(Memory_Chunk);

	return chunk;
}

def(void, Release, Memory_Chunk *chunk) {
	munmap(chunk, chunk->size + sizeof(Memory_Chunk));
}

def(Memory_Chunk *, Resize, Memory_Chunk *chunk, size_t size) {
	size_t old = chunk->size + sizeof(Memory_Chunk);
	size_t new = scall(AlignSize, size + sizeof(Memory_Chunk), Memory_PageSize);

	if (old == new) {
		return chunk;
	}

	chunk = mremap(chunk, old, new, MREMAP_MAYMOVE);

	if (chunk == MAP_FAILED) {
		return NULL;
	}

	chunk->size = new - sizeof(Memory_Chunk);

	return chunk;
}

Impl(Memory) = {
	.allocate = ref(Allocate),
	.release  = ref(Release),
	.resize   = ref(Resize)
};
