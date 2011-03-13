#import "Directory.h"

#define self Directory

def(void, Init, RdString path) {
	this->fd = Kernel_open(path,
		FileStatus_Directory |
		FileStatus_ReadOnly, 0);

	if (this->fd == -1) {
		throw(CannotOpenDirectory);
	}

	this->bpos = 0;
}

def(void, Destroy) {
	Kernel_close(this->fd);
}

def(bool, Read, ref(Entry) *res) {
	if (this->bpos == 0 || this->bpos >= this->nread) {
		this->nread = Kernel_getdents(this->fd, this->buf, ref(BufSize));
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
