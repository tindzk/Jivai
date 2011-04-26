#import <EventQueue.h>

#import "TestSuite.h"

#define self tsEventQueue

class {
	EventQueue queue;
};

tsRegister("EventQueue") {
	return true;
}

tsInit {
	this->queue = EventQueue_New();
}

tsDestroy {
	EventQueue_Destroy(&this->queue);
}

tsCase(Acute, "HasEvents()") {
	Assert($("Is empty"), !EventQueue_HasEvents(&this->queue));

	EventQueue_Enqueue(&this->queue, 1234, 1);
	EventQueue_Enqueue(&this->queue, 2345, 2);

	Assert($("Not empty"), EventQueue_HasEvents(&this->queue));

	EventQueue_Pop(&this->queue);
	EventQueue_Pop(&this->queue);

	Assert($("Is empty"), !EventQueue_HasEvents(&this->queue));
}

tsCase(Acute, "Prune()") {
	EventQueue_Enqueue(&this->queue, 2345, 1);
	EventQueue_Enqueue(&this->queue, 1234, 2);
	EventQueue_Enqueue(&this->queue, 2345, 1);
	EventQueue_Enqueue(&this->queue, 1234, 2);

	EventQueue_Prune(&this->queue, 1234);

	Assert($("Not empty"), EventQueue_HasEvents(&this->queue));

	EventQueue_Prune(&this->queue, 2345);

	Assert($("Is empty"), !EventQueue_HasEvents(&this->queue));
}

tsFinalize;
