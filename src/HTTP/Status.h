#ifndef HTTP_STATUS_H
#define HTTP_STATUS_H

#include "../String.h"

typedef struct {
	int code;
	String msg;
	String descr;
} HTTP_Status;

#define HTTP_Status(code, msg, descr) \
	{ code, StaticString(msg), StaticString(descr) }

extern HTTP_Status
	HTTP_Status_Info_Continue,
	HTTP_Status_Info_SwitchingProtocol,
	HTTP_Status_Info_Processing,

	HTTP_Status_Success_Ok,
	HTTP_Status_Success_Created,
	HTTP_Status_Success_Accepted,
	HTTP_Status_Success_NonAuthoritative,
	HTTP_Status_Success_NoContent,
	HTTP_Status_Success_ResetContent,
	HTTP_Status_Success_PartialContent,
	HTTP_Status_Success_MultiStatus,

	HTTP_Status_Redirection_MultipleChoices,
	HTTP_Status_Redirection_Permanent,
	HTTP_Status_Redirection_Found,
	HTTP_Status_Redirection_SeeOther,
	HTTP_Status_Redirection_NotModified,
	HTTP_Status_Redirection_UseProxy,
	HTTP_Status_Redirection_Temporary,

	HTTP_Status_ClientError_BadRequest,
	HTTP_Status_ClientError_Unauthorized,
	HTTP_Status_ClientError_PaymentRequired,
	HTTP_Status_ClientError_Forbidden,
	HTTP_Status_ClientError_NotFound,
	HTTP_Status_ClientError_MethodNotAllowed,
	HTTP_Status_ClientError_NotAcceptable,
	HTTP_Status_ClientError_ProxyAuthentificationRequired,
	HTTP_Status_ClientError_RequestTimeout,
	HTTP_Status_ClientError_Conflict,
	HTTP_Status_ClientError_Gone,
	HTTP_Status_ClientError_LengthRequired,
	HTTP_Status_ClientError_PreconditionFailed,
	HTTP_Status_ClientError_RequestEntityTooLarge,
	HTTP_Status_ClientError_RequestUriTooLong,
	HTTP_Status_ClientError_UnsupportedMediaType,
	HTTP_Status_ClientError_RequestedRangeNotSatisfiable,
	HTTP_Status_ClientError_ExpectationFailed,
	HTTP_Status_ClientError_UnprocessableEntity,
	HTTP_Status_ClientErrorLocked,
	HTTP_Status_ClientError_FailedDependency,

	HTTP_Status_ServerError_Internal,
	HTTP_Status_ServerError_NotImplemented,
	HTTP_Status_ServerError_BadGateway,
	HTTP_Status_ServerError_ServiceUnavailable,
	HTTP_Status_ServerError_GatewayTimeout,
	HTTP_Status_ServerError_VersionNotSupported,
	HTTP_Status_ServerError_InsufficientStorage;

#endif
