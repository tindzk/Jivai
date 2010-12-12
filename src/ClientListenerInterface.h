#import "Client.h"
#import "ClientConnection.h"

Interface(ClientListener) {
	Method(void, onInit);
	Method(void, onDestroy);
	Method(bool, onClientConnect);
	Method(void, onClientAccept, ClientInstance);
	Method(ClientConnection_Status, onPull, ClientInstance);
	Method(ClientConnection_Status, onPush, ClientInstance);
	Method(void, onClientDisconnect, ClientInstance);
};
