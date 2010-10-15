#import "Kernel.h"

int Kernel_open(String path, int flags, int mode) {
	return syscall(__NR_open, String_ToNul(path), flags, mode);
}

bool Kernel_close(int fd) {
	return syscall(__NR_close, fd) == 0;
}

void Kernel_exit(int status) {
	syscall(__NR_exit, status);
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

bool Kernel_llseek(int fd, u64 offset, u64 *pos, int whence) {
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
#if defined(__NR_socket)
	return syscall(__NR_socket, namespace, style, protocol);
#else
	long args[] = { namespace, style, protocol };
	return syscall(__NR_socketcall, SYS_SOCKET, args);
#endif
}

bool Kernel_setsockopt(int fd, int level, int option, void *value, size_t size) {
#if defined(__NR_setsockopt)
	return syscall(__NR_setsockopt, fd, level, option, value, size) == 0;
#else
	long args[] = { fd, level, option, (long) value, size };
	return syscall(__NR_socketcall, SYS_SETSOCKOPT, args) == 0;
#endif
}

bool Kernel_bind(int fd, struct sockaddr_in addr) {
#if defined(__NR_bind)
	return syscall(__NR_bind, fd, &addr, sizeof(addr)) == 0;
#else
	long args[] = { fd, (long) &addr, sizeof(addr) };
	return syscall(__NR_socketcall, SYS_BIND, args) == 0;
#endif
}

bool Kernel_listen(int fd, int backlog) {
#if defined(__NR_listen)
	return syscall(__NR_listen, fd, backlog) == 0;
#else
	long args[] = { fd, backlog };
	return syscall(__NR_socketcall, SYS_LISTEN, args) == 0;
#endif
}

bool Kernel_shutdown(int fd, int how) {
#if defined(__NR_shutdown)
	return syscall(__NR_shutdown, fd, how) == 0;
#else
	long args[] = { fd, how };
	return syscall(__NR_socketcall, SYS_SHUTDOWN, args) == 0;
#endif
}

bool Kernel_connect(int fd, void *addr, size_t size) {
#if defined(__NR_connect)
	return syscall(__NR_connect, fd, addr, size) == 0;
#else
	long args[] = { fd, (long) addr, size };
	return syscall(__NR_socketcall, SYS_CONNECT, args) == 0;
#endif
}

ssize_t Kernel_accept(int fd, void *addr, void *len) {
#if defined(__NR_accept)
	return syscall(__NR_accept, fd, addr, len);
#else
	long args[] = { fd, (long) addr, (long) len };
	return syscall(__NR_socketcall, SYS_ACCEPT, args);
#endif
}

ssize_t Kernel_recv(int fd, void *buf, size_t len, int flags) {
#if defined(__NR_recvfrom)
	return syscall(__NR_recvfrom, fd, buf, len, flags, NULL, NULL);
#else
	long args[] = { fd, (long) buf, len, flags };
	return syscall(__NR_socketcall, SYS_RECV, args);
#endif
}

ssize_t Kernel_send(int fd, void *buf, size_t len, int flags) {
#if defined(__NR_sendto)
	return syscall(__NR_sendto, fd, buf, len, flags, NULL, 0);
#else
	long args[] = { fd, (long) buf, len, flags };
	return syscall(__NR_socketcall, SYS_SEND, args);
#endif
}

ssize_t Kernel_sendfile64(int out, int in, u64 *offset, size_t len) {
	return syscall(__NR_sendfile64, out, in, offset, len);
}
