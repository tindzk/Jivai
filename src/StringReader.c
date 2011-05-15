#import "StringReader.h"

#define self StringReader

rsdef(self, New, RdString s) {
	return (self) {
		.buf  = s,
		.line = 1
	};
}

overload def(bool, Peek, char *c) {
	assert(c != NULL);

	if (this->ofs >= this->buf.len) {
		*c = '\0';
		return false;
	}

	*c = this->buf.buf[this->ofs];

	return true;
}

overload def(bool, Peek, char *c, size_t cnt) {
	assert(c != NULL);

	if (this->ofs + cnt >= this->buf.len) {
		*c = '\0';
		return false;
	}

	*c = this->buf.buf[this->ofs + cnt];

	return true;
}

overload def(void, Consume) {
	assert(this->ofs + 1 <= this->buf.len);

	if (this->buf.buf[this->ofs] == '\n') {
		this->line++;
	}

	this->ofs++;
}

def(void, Extend, RdString *str) {
	assert(str != NULL);
	assert(this->ofs + 1 <= this->buf.len);

	if (str->len == 0) {
		*str = String_Slice(this->buf, this->ofs, 1);
	} else {
		size_t ofs = str->buf - this->buf.buf;
		assert(ofs + str->len + 1 <= this->buf.len);
		str->len++;
	}

	if (this->buf.buf[this->ofs] == '\n') {
		this->line++;
	}

	this->ofs++;
}
