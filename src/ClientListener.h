#import "Connection.h"
#import "SocketClient.h"

Interface(ClientListener) {
	void              (*onInit)       (GenericInstance);
	void              (*onDestroy)    (GenericInstance);
	bool              (*onConnect)    (GenericInstance);
	void              (*onDisconnect) (GenericInstance, SocketClientInstance);
	void              (*onAccept)     (GenericInstance, SocketClientInstance);
	Connection_Status (*onPull)       (GenericInstance, SocketClientInstance);
	Connection_Status (*onPush)       (GenericInstance, SocketClientInstance);
};
