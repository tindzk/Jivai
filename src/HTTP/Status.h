#import "../String.h"

#define self HTTP_Status

record(ref(Item)) {
	u16 code;
	RdString msg;
	RdString descr;
};

#define HTTP_Status_Item(code, msg, descr) \
	{ code, $(msg), $(descr) }

set(self) {
	ref(Unset),

	ref(Info_Continue),
	ref(Info_SwitchingProtocol),
	ref(Info_Processing),

	ref(Success_Ok),
	ref(Success_Created),
	ref(Success_Accepted),
	ref(Success_NonAuthoritative),
	ref(Success_NoContent),
	ref(Success_ResetContent),
	ref(Success_PartialContent),
	ref(Success_MultiStatus),

	ref(Redirection_MultipleChoices),
	ref(Redirection_Permanent),
	ref(Redirection_Found),
	ref(Redirection_SeeOther),
	ref(Redirection_NotModified),
	ref(Redirection_UseProxy),
	ref(Redirection_Temporary),

	ref(ClientError_BadRequest),
	ref(ClientError_Unauthorized),
	ref(ClientError_PaymentRequired),
	ref(ClientError_Forbidden),
	ref(ClientError_NotFound),
	ref(ClientError_MethodNotAllowed),
	ref(ClientError_NotAcceptable),
	ref(ClientError_ProxyAuthentificationRequired),
	ref(ClientError_RequestTimeout),
	ref(ClientError_Conflict),
	ref(ClientError_Gone),
	ref(ClientError_LengthRequired),
	ref(ClientError_PreconditionFailed),
	ref(ClientError_RequestEntityTooLarge),
	ref(ClientError_RequestUriTooLong),
	ref(ClientError_UnsupportedMediaType),
	ref(ClientError_RequestedRangeNotSatisfiable),
	ref(ClientError_ExpectationFailed),
	ref(ClientError_UnprocessableEntity),
	ref(ClientError_Locked),
	ref(ClientError_FailedDependency),

	ref(ServerError_Internal),
	ref(ServerError_NotImplemented),
	ref(ServerError_BadGateway),
	ref(ServerError_ServiceUnavailable),
	ref(ServerError_GatewayTimeout),
	ref(ServerError_VersionNotSupported),
	ref(ServerError_InsufficientStorage),

	ref(Length)
};

sdef(ref(Item), GetItem, self status);
sdef(self, GetStatusByCode, u16 code);

static inline sdef(bool, IsSuccess, ref(Item) status) {
	return status.code / 100 == 2;
}

static inline sdef(bool, IsRedirection, ref(Item) status) {
	return status.code / 100 == 3;
}

static inline sdef(bool, IsError, ref(Item) status) {
	return status.code / 100 == 4
		|| status.code / 100 == 5;
}

#undef self
