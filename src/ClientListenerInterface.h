#import "Client.h"
#import "Connection.h"

record(ClientListenerInterface) {
	void (*onInit)             (GenericInstance);
	void (*onDestroy)          (GenericInstance);
	bool (*onClientConnect)    (GenericInstance);
	void (*onClientAccept)     (GenericInstance, Client *);
	Connection_Status (*onPull)(GenericInstance, Client *);
	Connection_Status (*onPush)(GenericInstance, Client *);
	void (*onClientDisconnect) (GenericInstance, Client *);
};
