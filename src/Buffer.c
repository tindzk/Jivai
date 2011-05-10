#import "Buffer.h"
#import "Exception.h"

#define self Buffer

rsdef(self, New, size_t size) {
	assert(size > 0);

	return (self) {
		.ptr = Memory_New(size),
		.len = 0
	};
}

def(void, Destroy) {
	assert(this->ptr != NULL);

	Memory_Destroy(this->ptr);
	this->ptr = NULL;
}

rdef(size_t, GetSize) {
	return Memory_GetSize(this->ptr);
}

overload rdef(self, Clone) {
	assert(this->ptr != NULL);

	self res = scall(New, Memory_GetSize(this->ptr));
	Memory_Copy(res.ptr, this->ptr, this->len);
	res.len = this->len;

	return res;
}

overload rsdef(self, Clone, RdBuffer buf) {
	assert(buf.ptr != NULL);

	self res = scall(New, buf.len);
	Memory_Copy(res.ptr, buf.ptr, buf.len);
	res.len = buf.len;

	return res;
}

rsdef(bool, IsValid, RdBuffer buf) {
	return buf.ptr != NULL;
}

def(void, Resize, size_t size) {
	assert(size > 0);
	assert(this->ptr != NULL);

	this->ptr = Memory_Resize(this->ptr, size);
}

def(void, Align, size_t size) {
	assert(this->ptr != NULL);

	if (this->len >= size) {
		return;
	}

	call(Resize, size);
}

sdef(void, Move, WrBuffer dest, RdBuffer src) {
	assert(dest.size > 0 && dest.size >= src.len);
	assert(src.ptr != NULL && dest.ptr != NULL);

	Memory_Move(dest.ptr, src.ptr, src.len);
}

sdef(void, Copy, WrBuffer dest, RdBuffer src) {
	assert(dest.size > 0 && dest.size >= src.len);
	assert(src.ptr != NULL && dest.ptr != NULL);
	assert(!Memory_Overlaps(dest.ptr, src.ptr, src.len, src.len));

	Memory_Copy(dest.ptr, src.ptr, src.len);
}

overload rsdef(bool, Equals, RdBuffer buf1, RdBuffer buf2) {
	assert(buf1.ptr != NULL && buf2.ptr != NULL);

	return buf1.len == buf2.len
		&& Memory_Equals(buf1.ptr, buf2.ptr, buf1.len);
}

rdef(WrBuffer, AsWrBuffer) {
	assert(this->ptr != NULL);

	return (WrBuffer) {
		.ptr  = this->ptr,
		.size = Memory_GetSize(this->ptr)
	};
}
