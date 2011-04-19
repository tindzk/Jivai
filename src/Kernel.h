#import <linux/net.h>
#import <netinet/in.h>
#import <sys/syscall.h>

#import "Types.h"

#if defined(__x86_64__)
#define __NR_stat64      __NR_stat
#define __NR_fstat64     __NR_fstat
#define __NR_sendfile64  __NR_sendfile
#define __NR_truncate64  __NR_truncate
#define __NR_ftruncate64 __NR_ftruncate
#define __NR__llseek     __NR_lseek
#endif

set(ClockType) {
	/* System clock which measures the real time. May move forwards and
	 * backwards.
	 */
	ClockType_Realtime = 0,

	/* System clock which increases monotonically. Not affected by changes in
	 * the current system time.
	 */
	ClockType_Monotonic = 1,

	/* Per-process clock which measures the processor time spent by the process.
	 */
	ClockType_Process_CpuTimeId = 2,

	/* Per-thread clock which measures the processor time spent by the thread.
	 */
	ClockType_Thread_CpuTimeId = 3
};

set(FcntlMode) {
	FcntlMode_GetDescriptorFlags = 1,
	FcntlMode_SetDescriptorFlags = 2,
	FcntlMode_GetStatus          = 3,
	FcntlMode_SetStatus          = 4
};

set(FileDescriptorFlags) {
	FileDescriptorFlags_CloseOnExec = 1
};

set(FileStatus) {
	FileStatus_ReadOnly  = 00,
	FileStatus_WriteOnly = 01,
	FileStatus_ReadWrite = 02,
	FileStatus_Directory = 0200000,
	FileStatus_NoFollow  = 0400000,
	FileStatus_Append    = 02000,
	FileStatus_Create    = 0100,
	FileStatus_NonBlock  = 04000,
	FileStatus_Truncate  = 01000
};

set(FileMode) {
	FileMode_Mask        = 0170000,
	FileMode_Directory   = 0040000,
	FileMode_CharDevice  = 0020000,
	FileMode_BlockDevice = 0060000,
	FileMode_Regular     = 0100000,
	FileMode_FIFO        = 0010000,
	FileMode_Symlink     = 0120000,
	FileMode_Socket      = 0140000
};

set(FileAttribute) {
	FileAttribute_Sticky         = 01000,
	FileAttribute_ExecuteAsUser  = 04000,
	FileAttribute_ExecuteAsGroup = 02000
};

set(Permission) {
	Permission_OwnerMask    = 00700,
	Permission_OwnerRead    = 0400,
	Permission_OwnerWrite   = 0200,
	Permission_OwnerExecute = 0100,

	Permission_GroupMask    = 00070,
	Permission_GroupRead    = 00040,
	Permission_GroupWrite   = 00020,
	Permission_GroupExecute = 00010,

	Permission_OthersMask    = 00007,
	Permission_OthersRead    = 00004,
	Permission_OthersWrite   = 00002,
	Permission_OthersExecute = 00001
};

record(Time_UnixEpoch) {
	time_t sec;
	long   nsec;
};

record(Stat) {
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
	Time_UnixEpoch atime;
	Time_UnixEpoch mtime;
	Time_UnixEpoch ctime;
};

#if defined(__x86_64__)
record(Stat64) {
	__dev_t device;
	__ino64_t inode;
	__nlink_t hardlinks;
	__mode_t mode;
	__uid_t uid;
	__gid_t gid;
	int __pad0;
	__dev_t rdev;
	__off_t size;
	__blksize_t blksize;
	__blkcnt64_t blocks;
	Time_UnixEpoch atime;
	Time_UnixEpoch mtime;
	Time_UnixEpoch ctime;
	long int unused[3];
};
#else
record(Stat64) {
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
	Time_UnixEpoch atime;
	Time_UnixEpoch mtime;
	Time_UnixEpoch ctime;
	__ino64_t inode;
};
#endif

set(FileNo) {
	FileNo_StdIn  = 0,
	FileNo_StdOut = 1,
	FileNo_StdErr = 2
};

set(ExitStatus) {
	ExitStatus_Success = 0,
	ExitStatus_Failure = 1
};

set(EpollCtl) {
	EpollCtl_Add    = 1,
	EpollCtl_Delete = 2,
	EpollCtl_Modify = 3
};

#if defined(__x86_64__)
#define __epollPacked __packed
#else
#define __epollPacked
#endif

record(EpollEvent) {
	u32 events;

	union {
		void *ptr;
		int fd;
		u32 u32;
		u64 u64;
	};
} __epollPacked;

#import "String.h"

#define self Kernel

sdef(ssize_t, open, RdString path, int flags, int mode);
sdef(bool, close, ssize_t fd);
sdef(void, exit, int status);
sdef(ssize_t, read, ssize_t fd, char *buf, size_t len);
sdef(ssize_t, write, ssize_t fd, char *buf, size_t len);
sdef(ssize_t, getdents, ssize_t fd, char *buf, size_t len);
sdef(bool, llseek, ssize_t fd, u64 offset, u64 *pos, int whence);
sdef(bool, mkdir, RdString path, int flags);
sdef(bool, unlink, RdString path);
sdef(bool, rmdir, RdString path);
sdef(ssize_t, readlink, RdString path, char *buf, size_t size);
sdef(bool, setxattr, RdString path, RdString name, char *buf, size_t size, int flags);
sdef(bool, fsetxattr, ssize_t fd, RdString name, char *buf, size_t size, int flags);
sdef(ssize_t, getxattr, RdString path, RdString name, char *buf, size_t size);
sdef(ssize_t, fgetxattr, ssize_t fd, RdString name, char *buf, size_t size);
sdef(bool, symlink, RdString path1, RdString path2);
sdef(bool, utimensat, int dirfd, RdString path, Time_UnixEpoch t, int flags);
sdef(bool, chdir, RdString path);
sdef(bool, fchdir, ssize_t fd);
sdef(bool, lstat, RdString path, Stat *attr);
sdef(bool, stat, RdString path, Stat *attr);
sdef(size_t, getcwd, char *buf, size_t size);
sdef(bool, stat64, RdString path, Stat64 *attr);
sdef(bool, fstat64, ssize_t fd, Stat64 *attr);
sdef(bool, truncate64, RdString path, u64 len);
sdef(bool, ftruncate64, ssize_t fd, u64 len);
sdef(bool, clock_gettime, ClockType type, Time_UnixEpoch *res);
sdef(ssize_t, epoll_create, size_t n);
sdef(bool, epoll_ctl, ssize_t epfd, EpollCtl op, ssize_t fd, EpollEvent *event);
sdef(ssize_t, epoll_wait, ssize_t epfd, EpollEvent *events, int maxevents, int timeout);
sdef(int, fcntl, ssize_t fd, int cmd, int arg);
sdef(ssize_t, socket, int namespace, int style, int protocol);
sdef(bool, setsockopt, ssize_t fd, int level, int option, const void *value, int size);
sdef(bool, bind, ssize_t fd, struct sockaddr_in addr);
sdef(bool, listen, ssize_t fd, int backlog);
sdef(bool, shutdown, ssize_t fd, int how);
sdef(bool, connect, ssize_t fd, void *addr, size_t size);
sdef(ssize_t, accept, ssize_t fd, void *addr, int *len);
sdef(ssize_t, recv, ssize_t fd, void *buf, size_t len, int flags);
sdef(ssize_t, send, ssize_t fd, void *buf, size_t len, int flags);
sdef(ssize_t, sendfile64, ssize_t out, ssize_t in, u64 *offset, size_t len);

#undef self
