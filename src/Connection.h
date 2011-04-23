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

	void (*init)   (GenericInstance, struct Server_Client *, Logger *);
	void (*destroy)(GenericInstance);
	void (*pull)   (GenericInstance);
	void (*push)   (GenericInstance);
};

#undef self
