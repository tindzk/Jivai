#import "Kernel.h"

int Kernel_open(String path, int flags, int mode) {
	return syscall(__NR_open, String_ToNul(path), flags, mode);
}

bool Kernel_close(int fd) {
	return syscall(__NR_close, fd) == 0;
}

ssize_t Kernel_read(int fd, char *buf, size_t len) {
	return syscall(__NR_read, fd, buf, len);
}

ssize_t Kernel_write(int fd, char *buf, size_t len) {
	return syscall(__NR_write, fd, buf, len);
}

ssize_t Kernel_getdents(int fd, char *buf, size_t len) {
	return syscall(__NR_getdents, fd, buf, len);
}

bool Kernel_llseek(int fd, off64_t offset, off64_t *pos, int whence) {
	/* Conversion taken from dietlibc-0.32/lib/lseek64.c */
	return syscall(__NR__llseek,
		fd,
		(unsigned long) (offset >> 32),
		(unsigned long) offset & 0xffffffff,
		pos, whence) == 0;
}

bool Kernel_mkdir(String path, int flags) {
	return syscall(__NR_mkdir, String_ToNul(path), flags) == 0;
}

bool Kernel_unlink(String path) {
	return syscall(__NR_unlink, String_ToNul(path)) == 0;
}

bool Kernel_rmdir(String path) {
	return syscall(__NR_rmdir, String_ToNul(path)) == 0;
}

ssize_t Kernel_readlink(String path, char *buf, size_t size) {
	return syscall(__NR_readlink, String_ToNul(path), buf, size);
}

bool Kernel_setxattr(String path, String name, char *buf, size_t size, int flags) {
	return syscall(__NR_setxattr,
		String_ToNul(path),
		String_ToNul(name),
		buf, size, flags) == 0;
}

bool Kernel_fsetxattr(int fd, String name, char *buf, size_t size, int flags) {
	return syscall(__NR_fsetxattr, fd, String_ToNul(name), buf, size, flags) == 0;
}

ssize_t Kernel_getxattr(String path, String name, char *buf, size_t size) {
	return syscall(__NR_getxattr,
		String_ToNul(path),
		String_ToNul(name),
		buf, size);
}

ssize_t Kernel_fgetxattr(int fd, String name, char *buf, size_t size) {
	return syscall(__NR_fgetxattr, fd, String_ToNul(name), buf, size);
}

bool Kernel_symlink(String path1, String path2) {
	return syscall(__NR_readlink,
		String_ToNul(path1),
		String_ToNul(path2)) == 0;
}

bool Kernel_utimensat(int dirfd, String path, Time_UnixEpoch t, int flags) {
	return syscall(__NR_utimensat, dirfd,
		path,
		(const Time_UnixEpoch[2]) {t, t},
		flags) == 0;
}

bool Kernel_chdir(String path) {
	return syscall(__NR_chdir, String_ToNul(path)) == 0;
}

bool Kernel_fchdir(int fd) {
	return syscall(__NR_fchdir, fd) == 0;
}

bool Kernel_lstat(String path, Stat *attr) {
	return syscall(__NR_lstat, String_ToNul(path), attr) == 0;
}

bool Kernel_stat(String path, Stat *attr) {
	return syscall(__NR_stat, String_ToNul(path), attr) == 0;
}

size_t Kernel_getcwd(char *buf, size_t size) {
	return syscall(__NR_getcwd, buf, size);
}

bool Kernel_stat64(String path, Stat64 *attr) {
	return syscall(__NR_stat64, String_ToNul(path), attr) == 0;
}

bool Kernel_fstat64(int fd, Stat64 *attr) {
	return syscall(__NR_fstat64, fd, attr) == 0;
}

bool Kernel_truncate64(String path, size_t len) {
	return syscall(__NR_truncate64, String_ToNul(path), len) == 0;
}

bool Kernel_ftruncate64(int fd, size_t len) {
	return syscall(__NR_ftruncate64, fd, len) == 0;
}

bool Kernel_clock_gettime(int id, Time_UnixEpoch *res) {
	return syscall(__NR_clock_gettime, id, res) == 0;
}

ssize_t Kernel_epoll_create(size_t n) {
	return syscall(__NR_epoll_create, n);
}

bool Kernel_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event) {
	return syscall(__NR_epoll_ctl, epfd, op, fd, event) == 0;
}

ssize_t Kernel_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout) {
	return syscall(__NR_epoll_wait, epfd, events, maxevents, timeout);
}

ssize_t Kernel_fcntl(int fd, int cmd, int arg) {
	return syscall(__NR_fcntl, fd, cmd, arg);
}

ssize_t Kernel_socket(int namespace, int style, int protocol) {
	long args[] = { namespace, style, protocol };
	return syscall(__NR_socketcall, SYS_SOCKET, args);
}

bool Kernel_setsockopt(int fd, int level, int option, void *value, size_t size) {
	long args[] = { fd, level, option, (long) value, size };
	return syscall(__NR_socketcall, SYS_SETSOCKOPT, args) == 0;
}

bool Kernel_bind(int fd, struct sockaddr_in addr) {
	long args[] = { fd, (long) &addr, sizeof(addr) };
	return syscall(__NR_socketcall, SYS_BIND, args) == 0;
}

bool Kernel_listen(int fd, int backlog) {
	long args[] = { fd, backlog };
	return syscall(__NR_socketcall, SYS_LISTEN, args) == 0;
}

bool Kernel_shutdown(int fd, int how) {
	long args[] = { fd, how };
	return syscall(__NR_socketcall, SYS_SHUTDOWN, args) == 0;
}

bool Kernel_connect(int fd, void *addr, size_t size) {
	long args[] = { fd, (long) addr, size };
	return syscall(__NR_socketcall, SYS_CONNECT, args) == 0;
}

ssize_t Kernel_accept(int fd, void *addr, void *len) {
	long args[] = { fd, (long) addr, (long) len };
	return syscall(__NR_socketcall, SYS_ACCEPT, args);
}

ssize_t Kernel_recv(int fd, void *buf, size_t len, int flags) {
	long args[] = { fd, (long) buf, len, flags };
	return syscall(__NR_socketcall, SYS_RECV, args);
}

ssize_t Kernel_send(int fd, void *buf, size_t len, int flags) {
	long args[] = { fd, (long) buf, len, flags };
	return syscall(__NR_socketcall, SYS_SEND, args);
}

ssize_t Kernel_sendfile64(int out, int in, off64_t *offset, size_t len) {
	return syscall(__NR_sendfile64, out, in, offset, len);
}
