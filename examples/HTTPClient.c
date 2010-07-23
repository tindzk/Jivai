#include <Socket.h>
#include <SocketConnection.h>
#include <ExceptionManager.h>

ExceptionManager exc;

void HTTP_Client_GetResponse(String hostname, String path, short port, String *resp) {
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
		hostname
	);

	try (&exc) {
		SocketConnection_Write(&conn, request.buf, request.len);
	} catchAny(e) {
		cleanup(&socket, &conn);
		excRethrow;
	} finally {
		String_Destroy(&request);
	} tryEnd;

	while (true) {
		try (&exc) {
			size_t len = SocketConnection_Read(&conn,
				resp->buf  + resp->len,
				resp->size - resp->len
			);

			if (len == 0) {
				break;
			}

			resp->len += len;
		} catch(&SocketConnection_ConnectionResetException, e) {
			excBreak;
		} finally {

		} tryEnd;
	}

	cleanup(&conn, &socket);
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

	return EXIT_SUCCESS;
}
