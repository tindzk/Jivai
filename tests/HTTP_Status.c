#import <HTTP/Status.h>

#import "TestSuite.h"

#define self tsHTTP_Status

class {

};

tsRegister("HTTP.Status") {
	return true;
}

tsCase(Acute, "General") {
	Assert($("Unset"), HTTP_Status_Unset == 0);
}

tsCase(Acute, "Success") {
	Assert($("Ok"),
		HTTP_Status_IsSuccess(
			HTTP_Status_GetItem(
				HTTP_Status_Success_Ok)));

	Assert($("Accepted"),
		HTTP_Status_IsSuccess(
			HTTP_Status_GetItem(
				HTTP_Status_Success_Accepted)));

	Assert($("Found"),
		!HTTP_Status_IsSuccess(
			HTTP_Status_GetItem(
				HTTP_Status_Redirection_Found)));
}

tsCase(Acute, "Redirection") {
	Assert($("Ok"),
		!HTTP_Status_IsRedirection(
			HTTP_Status_GetItem(
				HTTP_Status_Success_Ok)));

	Assert($("Redirect"),
		HTTP_Status_IsRedirection(
			HTTP_Status_GetItem(
				HTTP_Status_Redirection_Found)));
}

tsCase(Acute, "Error") {
	Assert($("Gone"),
		HTTP_Status_IsError(
			HTTP_Status_GetItem(
				HTTP_Status_ClientError_Gone)));

	Assert($("Internal"),
		HTTP_Status_IsError(
			HTTP_Status_GetItem(
				HTTP_Status_ServerError_Internal)));
}
