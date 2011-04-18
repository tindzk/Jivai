#import <HTTP/Client.h>

int main(void) {
	HTTP_Client client;
	HTTP_Client_Init(&client, String_ToCarrier($$("www.kernel.org")));

	HTTP_Client_Request(&client,
		HTTP_Client_CreateRequest(
			String_ToCarrier($$("www.kernel.org")),
			String_ToCarrier($$("/"))));

	HTTP_Client_FetchResponse(&client);

	String buf = HTTP_Client_Read(&client, 50 * 1024);

	String_Print(buf.rd);
	String_Destroy(&buf);

	HTTP_Client_Destroy(&client);
}
