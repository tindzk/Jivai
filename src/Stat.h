#ifndef STAT_H
#define STAT_H

#include <fcntl.h>

#include "Date.h"

typedef struct {
	__dev_t device;
	unsigned short int __pad1;
	__ino_t inode;
	__mode_t mode;
	__nlink_t hardlinks;
	__uid_t uid;
	__gid_t gid;
	__dev_t rdev;
	unsigned short int __pad2;
	__off_t size;
	__blksize_t blksize;
	__blkcnt_t blocks;
	Date_UnixTime atime;
	Date_UnixTime mtime;
	Date_UnixTime ctime;
} Stat;

typedef struct {
	__dev_t device;
	unsigned int __pad1;
	__ino_t __inode;
	__mode_t mode;
	__nlink_t hardlinks;
	__uid_t uid;
	__gid_t gid;
	__dev_t rdev;
	unsigned int __pad2;
	__off64_t size;
	__blksize_t blksize;
	__blkcnt64_t blocks;
	Date_UnixTime atime;
	Date_UnixTime mtime;
	Date_UnixTime ctime;
	__ino64_t inode;
} Stat64;

typedef __off64_t off64_t;

#endif
