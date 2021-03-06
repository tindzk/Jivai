#import <Socket.h>
#import <Integer.h>
#import <SocketConnection.h>

int main(void) {
	Socket socket = Socket_New(Socket_Protocol_TCP);

	SocketConnection conn =
		Socket_Connect(&socket, $("www.kernel.org"), 80);

	String d1, d2;

	String_Print(
		d1 = String_Format(
			$("Connected to %:%\n\n"),
			d2 = NetworkAddress_ToString(conn.addr),
			Int16_ToString(conn.addr.port)));

	String_Destroy(&d2);
	String_Destroy(&d1);

	String request = $(
		"GET / HTTP/1.1\r\n"
		"Host: www.kernel.org\r\n"
		"Connection: Close\r\n"
		"\r\n");

	SocketConnection_Write(&conn, request.buf, request.len);

	String response = StackString(2048);

	response.len = SocketConnection_Read(&conn, response.buf, response.size);

	String_Print(response);

	SocketConnection_Close(&conn);

	Socket_Destroy(&socket);

	return ExitStatus_Success;
}
