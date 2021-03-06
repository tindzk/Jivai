#import "NetworkAddress.h"

#define self NetworkAddress

sdef(struct in_addr, ResolveHost, RdString hostname) {
	struct addrinfo *host;

	if (getaddrinfo(String_ToNul(hostname), NULL, NULL, &host)) {
		throw(GetAddrInfoFailed);
	}

	struct sockaddr_in saddr = *((struct sockaddr_in *) host->ai_addr);

	freeaddrinfo(host);

	return saddr.sin_addr;
}

/* Taken from diet libc (dietlibc-0.32/libcruft/inet_ntoa_r.c). */
static size_t i2a(char *dest, size_t x) {
	size_t tmp = x;
	size_t len = 0;

	if (x >= 100) {
		*dest++ = tmp / 100 + '0';
		tmp = tmp % 100;
		len++;
	}

	if (x >= 10) {
		*dest++ = tmp / 10 + '0';
		tmp = tmp % 10;
		len++;
	}

	*dest++ = tmp + '0';

	return len + 1;
}

/* Taken from diet libc (dietlibc-0.32/libcruft/inet_ntoa_r.c). */
sdef(String, ToString, NetworkAddress addr) {
	String out = String_New(15);

	unsigned char *ip = (unsigned char *) &addr.ip;

	out.len = i2a(out.buf, ip[0]);
	out.buf[out.len] = '.';
	out.len++;

	out.len += i2a(out.buf + out.len, ip[1]);

	out.buf[out.len] = '.';
	out.len++;

	out.len += i2a(out.buf + out.len, ip[2]);

	out.buf[out.len] = '.';
	out.len++;

	out.len += i2a(out.buf + out.len, ip[3]);

	return out;
}
