#import "SocketClient.h"
#import "ClientConnection.h"

Interface(ClientListener) {
	Method(void, onInit);
	Method(void, onDestroy);
	Method(bool, onClientConnect);
	Method(void, onClientAccept, SocketClientInstance);
	Method(ClientConnection_Status, onPull, SocketClientInstance);
	Method(ClientConnection_Status, onPush, SocketClientInstance);
	Method(void, onClientDisconnect, SocketClientInstance);
};
