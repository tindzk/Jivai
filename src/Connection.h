#import "Logger.h"
#import "SocketConnection.h"

#define self Connection

set(ref(Status)) {
	ref(Status_Close),
	ref(Status_Open)
};

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

Interface(self) {
	size_t size;

	void        (*init)   (GenericInstance, ref(Client) *, Logger *);
	void        (*destroy)(GenericInstance);
	ref(Status) (*pull)   (GenericInstance);
	ref(Status) (*push)   (GenericInstance);
};

#undef self
