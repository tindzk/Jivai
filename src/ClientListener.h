#import "SocketClient.h"
#import "ClientConnection.h"

Interface(ClientListener) {
	void (*onInit)                    (GenericInstance);
	void (*onDestroy)                 (GenericInstance);
	bool (*onClientConnect)           (GenericInstance);
	void (*onClientAccept)            (GenericInstance, SocketClientInstance);
	ClientConnection_Status (*onPull) (GenericInstance, SocketClientInstance);
	ClientConnection_Status (*onPush) (GenericInstance, SocketClientInstance);
	void (*onClientDisconnect)        (GenericInstance, SocketClientInstance);
};
