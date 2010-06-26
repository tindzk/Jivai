#include "Directory.h"

Exception_Define(Directory_CannotOpenDirectoryException);
Exception_Define(Directory_ReadingFailedException);

static ExceptionManager *exc;

void Directory0(ExceptionManager *e) {
	exc = e;
}

void Directory_Init(Directory *this, String path) {
	this->fd = open(String_ToNul(&path), O_RDONLY | O_DIRECTORY);

	if (this->fd == -1) {
		throw(exc, &Directory_CannotOpenDirectoryException);
	}

	this->bpos = 0;
}

void Directory_Destroy(Directory *this) {
	close(this->fd);
}

bool Directory_Read(Directory *this, Directory_Entry *res) {
	if (this->bpos == 0 || this->bpos >= this->nread) {
		this->nread = syscall(SYS_getdents, this->fd, this->buf, Directory_BufSize);
		this->bpos  = 0;

		if (this->nread == -1) {
			throw(exc, &Directory_ReadingFailedException);
		}

		if (this->nread == 0) {
			return false;
		}
	}

	char *p = this->buf + this->bpos;

	Directory_LinuxEntry *item = (Directory_LinuxEntry *) p;

	res->inode = item->inode;
	res->type  = *(p + item->reclen - 1);
	res->name  = StackString(0);
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