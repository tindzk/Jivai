#import "Logger.h"
#import "SocketConnection.h"

#define self Connection

set(ref(State)) {
	ref(State_Established), /* Nothing received/sent yet. */
	ref(State_Receiving),   /* Receiving data.            */
	ref(State_Sending),     /* Sending data.              */
	ref(State_Processing),  /* Processing input.          */
	ref(State_Idle)         /* No activity on both ends.  */
};

record(ref(Client)) {
	ref(State)       state;
	SocketConnection *conn;
};

/* pull = receive data and begin sending the response
 * push = send the rest of the response
 */

struct Server_Client;

Interface(self) {
	size_t size;

	void (*init)   (Instance $this, struct Server_Client *client, Logger *logger);
	void (*destroy)(Instance $this);
	void (*pull)   (Instance $this);
	void (*push)   (Instance $this);
};

#undef self
