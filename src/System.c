#import "System.h"

#define self System

/* We use __environ allowing us to use this function in constructors. */
sdef(bool, IsRunningOnValgrind) {
	assert(__environ != NULL);

	for (char **cur = __environ; *cur != NULL; cur++) {
		RdString item = String_FromNul(*cur);

		if (String_Equals(item, $("_=/usr/bin/valgrind"))) {
			return true;
		}
	}

	return false;
}
