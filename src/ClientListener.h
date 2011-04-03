#import "Connection.h"
#import "SocketClient.h"

Interface(ClientListener) {
	void (*onInit)              (GenericInstance);
	void (*onDestroy)           (GenericInstance);
	bool (*onClientConnect)     (GenericInstance);
	void (*onClientAccept)      (GenericInstance, SocketClientInstance);
	Connection_Status (*onPull) (GenericInstance, SocketClientInstance);
	Connection_Status (*onPush) (GenericInstance, SocketClientInstance);
	void (*onClientDisconnect)  (GenericInstance, SocketClientInstance);
};
