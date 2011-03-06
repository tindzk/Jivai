#import "Kernel.h"

#define self Kernel

sdef(ssize_t, open, ProtString path, int flags, int mode) {
	return syscall(__NR_open, String_ToNul(path), flags, mode);
}

sdef(bool, close, ssize_t fd) {
	return syscall(__NR_close, fd) == 0;
}

sdef(void, exit, int status) {
	syscall(__NR_exit, status);
}

sdef(ssize_t, read, ssize_t fd, char *buf, size_t len) {
	return syscall(__NR_read, fd, buf, len);
}

sdef(ssize_t, write, ssize_t fd, char *buf, size_t len) {
	return syscall(__NR_write, fd, buf, len);
}

sdef(ssize_t, getdents, ssize_t fd, char *buf, size_t len) {
	return syscall(__NR_getdents, fd, buf, len);
}

sdef(bool, llseek, ssize_t fd, u64 offset, u64 *pos, int whence) {
	/* Conversion taken from dietlibc-0.32/lib/lseek64.c */
	return syscall(__NR__llseek,
		fd,
		(unsigned long) (offset >> 32),
		(unsigned long) offset & 0xffffffff,
		pos, whence) == 0;
}

sdef(bool, mkdir, ProtString path, int flags) {
	return syscall(__NR_mkdir, String_ToNul(path), flags) == 0;
}

sdef(bool, unlink, ProtString path) {
	return syscall(__NR_unlink, String_ToNul(path)) == 0;
}

sdef(bool, rmdir, ProtString path) {
	return syscall(__NR_rmdir, String_ToNul(path)) == 0;
}

sdef(ssize_t, readlink, ProtString path, char *buf, size_t size) {
	return syscall(__NR_readlink, String_ToNul(path), buf, size);
}

sdef(bool, setxattr, ProtString path, ProtString name, char *buf, size_t size, int flags) {
	return syscall(__NR_setxattr,
		String_ToNul(path),
		String_ToNul(name),
		buf, size, flags) == 0;
}

sdef(bool, fsetxattr, ssize_t fd, ProtString name, char *buf, size_t size, int flags) {
	return syscall(__NR_fsetxattr, fd, String_ToNul(name), buf, size, flags) == 0;
}

sdef(ssize_t, getxattr, ProtString path, ProtString name, char *buf, size_t size) {
	return syscall(__NR_getxattr,
		String_ToNul(path),
		String_ToNul(name),
		buf, size);
}

sdef(ssize_t, fgetxattr, ssize_t fd, ProtString name, char *buf, size_t size) {
	return syscall(__NR_fgetxattr, fd, String_ToNul(name), buf, size);
}

sdef(bool, symlink, ProtString path1, ProtString path2) {
	return syscall(__NR_readlink,
		String_ToNul(path1),
		String_ToNul(path2)) == 0;
}

sdef(bool, utimensat, int dirfd, ProtString path, Time_UnixEpoch t, int flags) {
	return syscall(__NR_utimensat, dirfd,
		String_ToNul(path),
		(const Time_UnixEpoch[2]) {t, t},
		flags) == 0;
}

sdef(bool, chdir, ProtString path) {
	return syscall(__NR_chdir, String_ToNul(path)) == 0;
}

sdef(bool, fchdir, ssize_t fd) {
	return syscall(__NR_fchdir, fd) == 0;
}

sdef(bool, lstat, ProtString path, Stat *attr) {
	return syscall(__NR_lstat, String_ToNul(path), attr) == 0;
}

sdef(bool, stat, ProtString path, Stat *attr) {
	return syscall(__NR_stat, String_ToNul(path), attr) == 0;
}

sdef(size_t, getcwd, char *buf, size_t size) {
	return syscall(__NR_getcwd, buf, size);
}

sdef(bool, stat64, ProtString path, Stat64 *attr) {
	return syscall(__NR_stat64, String_ToNul(path), attr) == 0;
}

sdef(bool, fstat64, ssize_t fd, Stat64 *attr) {
	return syscall(__NR_fstat64, fd, attr) == 0;
}

sdef(bool, truncate64, ProtString path, u64 len) {
	return syscall(__NR_truncate64, String_ToNul(path), len) == 0;
}

sdef(bool, ftruncate64, ssize_t fd, u64 len) {
	return syscall(__NR_ftruncate64, fd, len) == 0;
}

sdef(bool, clock_gettime, int id, Time_UnixEpoch *res) {
	return syscall(__NR_clock_gettime, id, res) == 0;
}

sdef(ssize_t, epoll_create, size_t n) {
	return syscall(__NR_epoll_create, n);
}

sdef(bool, epoll_ctl, ssize_t epfd, EpollCtl op, ssize_t fd, EpollEvent *event) {
	return syscall(__NR_epoll_ctl, epfd, op, fd, event) == 0;
}

sdef(ssize_t, epoll_wait, ssize_t epfd, EpollEvent *events, int maxevents, int timeout) {
	/* TODO On x86, using syscall() causes a segmentation fault when this
	 * function is interrupted (e.g. by pressing Ctrl-C) and the return
	 * addresses of the trace are requested (Backtrace_GetReturnAddr()).
	 *
	 * epoll_wait() appears to circumvent this issue.
	 */
	#include <sys/epoll.h>
	return epoll_wait(epfd, (void *) events, maxevents, timeout);
	// return syscall(__NR_epoll_wait, epfd, events, maxevents, timeout);
}

sdef(int, fcntl, ssize_t fd, int cmd, int arg) {
	return syscall(__NR_fcntl, fd, cmd, arg);
}

sdef(ssize_t, socket, int namespace, int style, int protocol) {
#if defined(__NR_socket)
	return syscall(__NR_socket, namespace, style, protocol);
#else
	long args[] = { namespace, style, protocol };
	return syscall(__NR_socketcall, SYS_SOCKET, args);
#endif
}

sdef(bool, setsockopt, ssize_t fd, int level, int option, const void *value, int size) {
#if defined(__NR_setsockopt)
	return syscall(__NR_setsockopt, fd, level, option, value, size) == 0;
#else
	long args[] = { fd, level, option, (long) value, size };
	return syscall(__NR_socketcall, SYS_SETSOCKOPT, args) == 0;
#endif
}

sdef(bool, bind, ssize_t fd, struct sockaddr_in addr) {
#if defined(__NR_bind)
	return syscall(__NR_bind, fd, &addr, sizeof(addr)) == 0;
#else
	long args[] = { fd, (long) &addr, sizeof(addr) };
	return syscall(__NR_socketcall, SYS_BIND, args) == 0;
#endif
}

sdef(bool, listen, ssize_t fd, int backlog) {
#if defined(__NR_listen)
	return syscall(__NR_listen, fd, backlog) == 0;
#else
	long args[] = { fd, backlog };
	return syscall(__NR_socketcall, SYS_LISTEN, args) == 0;
#endif
}

sdef(bool, shutdown, ssize_t fd, int how) {
#if defined(__NR_shutdown)
	return syscall(__NR_shutdown, fd, how) == 0;
#else
	long args[] = { fd, how };
	return syscall(__NR_socketcall, SYS_SHUTDOWN, args) == 0;
#endif
}

sdef(bool, connect, ssize_t fd, void *addr, size_t size) {
#if defined(__NR_connect)
	return syscall(__NR_connect, fd, addr, size) == 0;
#else
	long args[] = { fd, (long) addr, size };
	return syscall(__NR_socketcall, SYS_CONNECT, args) == 0;
#endif
}

sdef(ssize_t, accept, ssize_t fd, void *addr, int *len) {
#if defined(__NR_accept)
	return syscall(__NR_accept, fd, addr, len);
#else
	long args[] = { fd, (long) addr, (long) len };
	return syscall(__NR_socketcall, SYS_ACCEPT, args);
#endif
}

sdef(ssize_t, recv, ssize_t fd, void *buf, size_t len, int flags) {
#if defined(__NR_recvfrom)
	return syscall(__NR_recvfrom, fd, buf, len, flags, NULL, NULL);
#else
	long args[] = { fd, (long) buf, len, flags };
	return syscall(__NR_socketcall, SYS_RECV, args);
#endif
}

sdef(ssize_t, send, ssize_t fd, void *buf, size_t len, int flags) {
#if defined(__NR_sendto)
	return syscall(__NR_sendto, fd, buf, len, flags, NULL, 0);
#else
	long args[] = { fd, (long) buf, len, flags };
	return syscall(__NR_socketcall, SYS_SEND, args);
#endif
}

sdef(ssize_t, sendfile64, ssize_t out, ssize_t in, u64 *offset, size_t len) {
	return syscall(__NR_sendfile64, out, in, offset, len);
}
