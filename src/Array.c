#include "Array.h"

void Array_CustomInit(Array *this, size_t len, size_t size) {
	if (len > 0) {
		this->buf = Memory_Alloc(len * size);
	} else {
		this->buf = NULL;
	}

	this->len  = 0;
	this->size = len;
}

void Array_CustomDestroy(Array *this) {
	this->len  = 0;
	this->size = 0;

	if (this->buf != NULL) {
		Memory_Free(this->buf);
	}
}

void Array_CustomResize(Array *this, size_t len, size_t size) {
	if (this->buf == NULL) {
		this->buf = Memory_Alloc(len * size);
	} else {
		this->buf = Memory_Realloc(this->buf, len * size);
	}

	this->size = len;

	if (this->len > len) {
		this->len = len;
	}
}

void Array_CustomAlign(Array *this, size_t len, size_t size) {
	if (len > 0) {
		if (this->len == 0 || len > this->size) {
			Array_CustomResize(this, len, size);
		}
	}
}
