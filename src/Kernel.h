#import <linux/net.h>
#import <sys/epoll.h>
#import <sys/syscall.h>
#import <netinet/in.h>

#import "Types.h"

#if defined(__x86_64__)
#define __NR_stat64      __NR_stat
#define __NR_fstat64     __NR_fstat
#define __NR_sendfile64  __NR_sendfile
#define __NR_truncate64  __NR_truncate
#define __NR_ftruncate64 __NR_ftruncate
#define __NR__llseek     __NR_lseek
#endif

typedef enum {
	FileStatus_ReadOnly  = 00,
	FileStatus_WriteOnly = 01,
	FileStatus_ReadWrite = 02,
	FileStatus_Directory = 0200000,
	FileStatus_NoFollow  = 0400000,
	FileStatus_Append    = 02000,
	FileStatus_Create    = 0100,
	FileStatus_NonBlock  = 04000,
	FileStatus_Truncate  = 01000
} FileStatus;

typedef enum {
	FileMode_Mask        = 0170000,
	FileMode_Directory   = 0040000,
	FileMode_CharDevice  = 0020000,
	FileMode_BlockDevice = 0060000,
	FileMode_Regular     = 0100000,
	FileMode_FIFO        = 0010000,
	FileMode_Symlink     = 0120000,
	FileMode_Socket      = 0140000
} FileModes;

typedef enum {
	FileAttribute_Sticky         = 01000,
	FileAttribute_ExecuteAsUser  = 04000,
	FileAttribute_ExecuteAsGroup = 02000
} FileAttributes;

typedef enum {
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
} Permissions;

typedef struct {
	time_t sec;
	long   nsec;
} Time_UnixEpoch;

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
	Time_UnixEpoch atime;
	Time_UnixEpoch mtime;
	Time_UnixEpoch ctime;
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
	Time_UnixEpoch atime;
	Time_UnixEpoch mtime;
	Time_UnixEpoch ctime;
	__ino64_t inode;
} Stat64;

typedef enum {
	FileNo_StdIn  = 0,
	FileNo_StdOut = 1,
	FileNo_StdErr = 2
} FileNo;

typedef __off64_t off64_t;

#import "String.h"

int Kernel_open(String path, int flags, int mode);
bool Kernel_close(int fd);
ssize_t Kernel_read(int fd, char *buf, size_t len);
ssize_t Kernel_write(int fd, char *buf, size_t len);
ssize_t Kernel_getdents(int fd, char *buf, size_t len);
bool Kernel_llseek(int fd, off64_t offset, off64_t *pos, int whence);
bool Kernel_mkdir(String path, int flags);
bool Kernel_unlink(String path);
bool Kernel_rmdir(String path);
ssize_t Kernel_readlink(String path, char *buf, size_t size);
bool Kernel_setxattr(String path, String name, char *buf, size_t size, int flags);
bool Kernel_fsetxattr(int fd, String name, char *buf, size_t size, int flags);
ssize_t Kernel_getxattr(String path, String name, char *buf, size_t size);
ssize_t Kernel_fgetxattr(int fd, String name, char *buf, size_t size);
bool Kernel_symlink(String path1, String path2);
bool Kernel_utimensat(int dirfd, String path, Time_UnixEpoch t, int flags);
bool Kernel_chdir(String path);
bool Kernel_fchdir(int fd);
bool Kernel_lstat(String path, Stat *attr);
bool Kernel_stat(String path, Stat *attr);
size_t Kernel_getcwd(char *buf, size_t size);
bool Kernel_stat64(String path, Stat64 *attr);
bool Kernel_fstat64(int fd, Stat64 *attr);
bool Kernel_truncate64(String path, size_t len);
bool Kernel_ftruncate64(int fd, size_t len);
bool Kernel_clock_gettime(int id, Time_UnixEpoch *res);
ssize_t Kernel_epoll_create(size_t n);
bool Kernel_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
ssize_t Kernel_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
ssize_t Kernel_fcntl(int fd, int cmd, int arg);
ssize_t Kernel_socket(int namespace, int style, int protocol);
bool Kernel_setsockopt(int fd, int level, int option, void *value, size_t size);
bool Kernel_bind(int fd, struct sockaddr_in addr);
bool Kernel_listen(int fd, int backlog);
bool Kernel_shutdown(int fd, int how);
bool Kernel_connect(int fd, void *addr, size_t size);
ssize_t Kernel_accept(int fd, void *addr, void *len);
ssize_t Kernel_recv(int fd, void *buf, size_t len, int flags);
ssize_t Kernel_send(int fd, void *buf, size_t len, int flags);
ssize_t Kernel_sendfile64(int out, int in, off64_t *offset, size_t len);
