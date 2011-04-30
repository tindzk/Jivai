#import "Directory.h"

#define self Directory

rsdef(self, New, RdString path) {
	int id = Kernel_open(path,
		FileStatus_Directory |
		FileStatus_ReadOnly, 0);

	if (id == -1) {
		throw(CannotOpenDirectory);
	}

	return (self) {
		.ch   = Channel_New(id, 0),
		.bpos = 0
	};
}

def(void, Destroy) {
	Channel_Destroy(&this->ch);
}

def(bool, Read, ref(Entry) *res) {
	if (this->bpos == 0 || this->bpos >= this->nread) {
		int id = Channel_GetId(&this->ch);

		this->nread = Kernel_getdents(id, this->buf, ref(BufSize));
		this->bpos  = 0;

		if (this->nread == -1) {
			throw(ReadingFailed);
		}

		if (this->nread == 0) {
			return false;
		}
	}

	char *p = this->buf + this->bpos;

	ref(LinuxEntry) *item = (ref(LinuxEntry) *) p;

	res->inode = item->inode;
	res->type  = *(p + item->reclen - 1);
	res->name  = $("");
	res->name.buf = item->name;

	/* TODO This does not work. */
	res->name.len = item->reclen
		- sizeof(item->inode)
		- sizeof(item->offset)
		- sizeof(item->reclen)
		- sizeof(item->name[0])
		- sizeof(char) /* pad */
		- sizeof(char) /* d_type */
		- 1; /* NUL terminator */

	/* Use strlen() for the time being. */
	res->name.len = strlen(res->name.buf);

	this->bpos += item->reclen;

	return true;
}
