#import <errno.h>
#import <linux/net.h>
#import <netinet/in.h>
#import <sys/syscall.h>

#import "Types.h"
#import "String.h"

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
		void *addr;
		u64 u64;
	};
} __epollPacked;

#define self Kernel

sdef(int, open, RdString path, int flags, int mode);
sdef(bool, close, int id);
sdef(void, exit, ExitStatus status);
sdef(ssize_t, read, int id, void *buf, size_t len);
sdef(ssize_t, write, int id, void *buf, size_t len);
sdef(ssize_t, getdents, int id, char *buf, size_t len);
sdef(bool, llseek, int id, u64 offset, u64 *pos, int whence);
sdef(bool, mkdir, RdString path, int flags);
sdef(bool, unlink, RdString path);
sdef(bool, rmdir, RdString path);
sdef(ssize_t, readlink, RdString path, char *buf, size_t size);
sdef(bool, setxattr, RdString path, RdString name, char *buf, size_t size, int flags);
sdef(bool, fsetxattr, int id, RdString name, char *buf, size_t size, int flags);
sdef(ssize_t, getxattr, RdString path, RdString name, char *buf, size_t size);
sdef(ssize_t, fgetxattr, int id, RdString name, char *buf, size_t size);
sdef(bool, symlink, RdString path1, RdString path2);
sdef(bool, utimensat, int dirId, RdString path, Time_UnixEpoch t, int flags);
sdef(bool, chdir, RdString path);
sdef(bool, fchdir, int id);
sdef(bool, lstat, RdString path, Stat *attr);
sdef(bool, stat, RdString path, Stat *attr);
sdef(size_t, getcwd, char *buf, size_t size);
sdef(bool, stat64, RdString path, Stat64 *attr);
sdef(bool, fstat64, int id, Stat64 *attr);
sdef(bool, truncate64, RdString path, u64 len);
sdef(bool, ftruncate64, int id, u64 len);
sdef(bool, clock_gettime, ClockType type, Time_UnixEpoch *res);
sdef(ssize_t, epoll_create, size_t n);
sdef(bool, epoll_ctl, int epollId, EpollCtl op, int id, EpollEvent *event);
sdef(ssize_t, epoll_wait, int epollId, EpollEvent *events, int maxevents, int timeout);
sdef(int, fcntl, int id, int cmd, int arg);
sdef(ssize_t, socket, int namespace, int style, int protocol);
sdef(bool, setsockopt, int id, int level, int option, const void *value, int size);
sdef(bool, bind, int id, struct sockaddr_in addr);
sdef(bool, listen, int id, int backlog);
sdef(bool, shutdown, int id, int how);
sdef(bool, connect, int id, void *addr, size_t size);
sdef(ssize_t, accept4, int id, void *addr, int *len, int flags);
sdef(ssize_t, recv, int id, void *buf, size_t len, int flags);
sdef(ssize_t, send, int id, void *buf, size_t len, int flags);
sdef(ssize_t, sendfile64, int outId, int inId, u64 *offset, size_t len);

#undef self
