#import "Kernel.h"
#import "UniStd.h" /* for syscall() */

#define self Kernel

sdef(int, open, RdString path, int flags, int mode) {
	return syscall(__NR_open, String_ToNul(path), flags, mode);
}

sdef(bool, close, int id) {
	return syscall(__NR_close, id) == 0;
}

sdef(void, exit, ExitStatus status) {
	syscall(__NR_exit, status);
}

sdef(ssize_t, read, int id, void *buf, size_t len) {
	return syscall(__NR_read, id, buf, len);
}

sdef(ssize_t, write, int id, void *buf, size_t len) {
	return syscall(__NR_write, id, buf, len);
}

sdef(ssize_t, getdents, int id, char *buf, size_t len) {
	return syscall(__NR_getdents, id, buf, len);
}

sdef(bool, llseek, int id, u64 offset, u64 *pos, int whence) {
	/* Conversion taken from dietlibc-0.32/lib/lseek64.c */
	return syscall(__NR__llseek,
		id,
		(unsigned long) (offset >> 32),
		(unsigned long) offset & 0xffffffff,
		pos, whence) == 0;
}

sdef(bool, mkdir, RdString path, int flags) {
	return syscall(__NR_mkdir, String_ToNul(path), flags) == 0;
}

sdef(bool, unlink, RdString path) {
	return syscall(__NR_unlink, String_ToNul(path)) == 0;
}

sdef(bool, rmdir, RdString path) {
	return syscall(__NR_rmdir, String_ToNul(path)) == 0;
}

sdef(ssize_t, readlink, RdString path, char *buf, size_t size) {
	return syscall(__NR_readlink, String_ToNul(path), buf, size);
}

sdef(bool, setxattr, RdString path, RdString name, char *buf, size_t size, int flags) {
	return syscall(__NR_setxattr,
		String_ToNul(path),
		String_ToNul(name),
		buf, size, flags) == 0;
}

sdef(bool, fsetxattr, int id, RdString name, char *buf, size_t size, int flags) {
	return syscall(__NR_fsetxattr, id, String_ToNul(name), buf, size, flags) == 0;
}

sdef(ssize_t, getxattr, RdString path, RdString name, char *buf, size_t size) {
	return syscall(__NR_getxattr,
		String_ToNul(path),
		String_ToNul(name),
		buf, size);
}

sdef(ssize_t, fgetxattr, int id, RdString name, char *buf, size_t size) {
	return syscall(__NR_fgetxattr, id, String_ToNul(name), buf, size);
}

sdef(bool, symlink, RdString path1, RdString path2) {
	return syscall(__NR_readlink,
		String_ToNul(path1),
		String_ToNul(path2)) == 0;
}

sdef(bool, utimensat, int dirId, RdString path, Time_UnixEpoch t, int flags) {
	return syscall(__NR_utimensat, dirId,
		String_ToNul(path),
		(const Time_UnixEpoch[2]) {t, t},
		flags) == 0;
}

sdef(bool, chdir, RdString path) {
	return syscall(__NR_chdir, String_ToNul(path)) == 0;
}

sdef(bool, fchdir, int id) {
	return syscall(__NR_fchdir, id) == 0;
}

sdef(bool, lstat, RdString path, Stat *attr) {
	return syscall(__NR_lstat, String_ToNul(path), attr) == 0;
}

sdef(bool, stat, RdString path, Stat *attr) {
	return syscall(__NR_stat, String_ToNul(path), attr) == 0;
}

sdef(size_t, getcwd, char *buf, size_t size) {
	return syscall(__NR_getcwd, buf, size);
}

sdef(bool, stat64, RdString path, Stat64 *attr) {
	return syscall(__NR_stat64, String_ToNul(path), attr) == 0;
}

sdef(bool, fstat64, int id, Stat64 *attr) {
	return syscall(__NR_fstat64, id, attr) == 0;
}

sdef(bool, truncate64, RdString path, u64 len) {
	return syscall(__NR_truncate64, String_ToNul(path), len) == 0;
}

sdef(bool, ftruncate64, int id, u64 len) {
	return syscall(__NR_ftruncate64, id, len) == 0;
}

sdef(bool, clock_gettime, ClockType type, Time_UnixEpoch *res) {
	return syscall(__NR_clock_gettime, type, res) == 0;
}

sdef(ssize_t, epoll_create, size_t n) {
	return syscall(__NR_epoll_create, n);
}

sdef(bool, epoll_ctl, int epollId, EpollCtl op, int id, EpollEvent *event) {
	return syscall(__NR_epoll_ctl, epollId, op, id, event) == 0;
}

sdef(ssize_t, epoll_wait, int epollId, EpollEvent *events, int maxevents, int timeout) {
	/* TODO On x86, using syscall() causes a segmentation fault when this
	 * function is interrupted (e.g. by pressing Ctrl-C) and the return
	 * addresses of the trace are requested (Backtrace_GetReturnAddr()).
	 *
	 * epoll_wait() appears to circumvent this issue.
	 */
	#include <sys/epoll.h>
	return epoll_wait(epollId, (void *) events, maxevents, timeout);
	// return syscall(__NR_epoll_wait, epollId, events, maxevents, timeout);
}

sdef(int, fcntl, int id, int cmd, int arg) {
	return syscall(__NR_fcntl, id, cmd, arg);
}

sdef(ssize_t, socket, int namespace, int style, int protocol) {
#if defined(__NR_socket)
	return syscall(__NR_socket, namespace, style, protocol);
#else
	long args[] = { namespace, style, protocol };
	return syscall(__NR_socketcall, SYS_SOCKET, args);
#endif
}

sdef(bool, setsockopt, int id, int level, int option, const void *value, int size) {
#if defined(__NR_setsockopt)
	return syscall(__NR_setsockopt, id, level, option, value, size) == 0;
#else
	long args[] = { id, level, option, (long) value, size };
	return syscall(__NR_socketcall, SYS_SETSOCKOPT, args) == 0;
#endif
}

sdef(bool, bind, int id, struct sockaddr_in addr) {
#if defined(__NR_bind)
	return syscall(__NR_bind, id, &addr, sizeof(addr)) == 0;
#else
	long args[] = { id, (long) &addr, sizeof(addr) };
	return syscall(__NR_socketcall, SYS_BIND, args) == 0;
#endif
}

sdef(bool, listen, int id, int backlog) {
#if defined(__NR_listen)
	return syscall(__NR_listen, id, backlog) == 0;
#else
	long args[] = { id, backlog };
	return syscall(__NR_socketcall, SYS_LISTEN, args) == 0;
#endif
}

sdef(bool, shutdown, int id, int how) {
#if defined(__NR_shutdown)
	return syscall(__NR_shutdown, id, how) == 0;
#else
	long args[] = { id, how };
	return syscall(__NR_socketcall, SYS_SHUTDOWN, args) == 0;
#endif
}

sdef(bool, connect, int id, void *addr, size_t size) {
#if defined(__NR_connect)
	return syscall(__NR_connect, id, addr, size) == 0;
#else
	long args[] = { id, (long) addr, size };
	return syscall(__NR_socketcall, SYS_CONNECT, args) == 0;
#endif
}

sdef(ssize_t, accept4, int id, void *addr, int *len, int flags) {
#if defined(__NR_accept4)
	return syscall(__NR_accept4, id, addr, len, flags);
#else
	long args[] = { id, (long) addr, (long) len, (long) flags };
	return syscall(__NR_socketcall, SYS_ACCEPT4, args);
#endif
}

sdef(ssize_t, recv, int id, void *buf, size_t len, int flags) {
#if defined(__NR_recvfrom)
	return syscall(__NR_recvfrom, id, buf, len, flags, NULL, NULL);
#else
	long args[] = { id, (long) buf, len, flags };
	return syscall(__NR_socketcall, SYS_RECV, args);
#endif
}

sdef(ssize_t, send, int id, void *buf, size_t len, int flags) {
#if defined(__NR_sendto)
	return syscall(__NR_sendto, id, buf, len, flags, NULL, 0);
#else
	long args[] = { id, (long) buf, len, flags };
	return syscall(__NR_socketcall, SYS_SEND, args);
#endif
}

sdef(ssize_t, sendfile64, int outId, int inId, u64 *offset, size_t len) {
	return syscall(__NR_sendfile64, outId, inId, offset, len);
}
