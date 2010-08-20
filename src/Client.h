#import "Socket.h"

typedef struct {
	SocketConnection *conn;
	void *data;
} Client;

Client* Client_New(void);
void Client_Destroy(Client *client);
void Client_Accept(Client *client, Socket *socket);
