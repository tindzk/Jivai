#import "Client.h"
#import "Connection.h"

record(ClientListenerInterface) {
	void (*onInit)             (GenericInstance);
	void (*onDestroy)          (GenericInstance);
	bool (*onClientConnect)    (GenericInstance);
	void (*onClientAccept)     (GenericInstance, ClientInstance);
	Connection_Status (*onPull)(GenericInstance, ClientInstance);
	Connection_Status (*onPush)(GenericInstance, ClientInstance);
	void (*onClientDisconnect) (GenericInstance, ClientInstance);
};
