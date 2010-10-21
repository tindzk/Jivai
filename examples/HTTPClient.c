#import <Block.h>
#import <Socket.h>
#import <SocketConnection.h>

ExceptionManager exc;

bool HTTP_Client_Receive(SocketConnection *conn, String *resp) {
	try (&exc) {
		size_t len = SocketConnection_Read(conn,
			resp->buf  + resp->len,
			resp->size - resp->len);

		if (len == 0) {
			excReturn false;
		}

		resp->len += len;
	} clean catch (SocketConnection, excConnectionReset) {
		excReturn false;
	} finally {

	} tryEnd;

	return true;
}

void HTTP_Client_GetResponse(String hostname, String path, unsigned short port, String *resp) {
	Socket socket;
	Socket_Init(&socket, Socket_Protocol_TCP);

	SocketConnection conn = Socket_Connect(&socket, hostname, port);

	typedef void (^CleanupBlock)(void *, void *);

	CleanupBlock cleanup = ^(void *a, void *b) {
		SocketConnection_Close(a);
		Socket_Destroy(b);
	};

	String request = String_Format(
		String(
			"GET % HTTP/1.1\r\n"
			"Host: %\r\n"
			"Connection: Close\r\n"
			"\r\n"),
		path,
		hostname);

	try (&exc) {
		SocketConnection_Write(&conn, request.buf, request.len);
	} clean finally {
		cleanup(&socket, &conn);
		String_Destroy(&request);
	} tryEnd;

	while (HTTP_Client_Receive(&conn, resp));
}

int main(void) {
	ExceptionManager_Init(&exc);

	Socket0(&exc);
	NetworkAddress0(&exc);
	SocketConnection0(&exc);

	String resp = HeapString(1024 * 150); // Fetch max. 150 KiB

	HTTP_Client_GetResponse(String("www.kernel.org"), String("/"), 80, &resp);

	String_Print(resp);

	String_Destroy(&resp);

	return ExitStatus_Success;
}
