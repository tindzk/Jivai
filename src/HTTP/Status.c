/*
 * Nanogear - C++ web development framework
 *
 * This library is based on Restlet (R) <http://www.restlet.org> by Noelios Technologies
 * Copyright (C) 2005-2008 by Noelios Technologies <http://www.noelios.com>
 * Restlet is a registered trademark of Noelios Technologies. All other marks and
 * trademarks are property of their respective owners.
 *
 * Copyright (C) 2008-2009 Lorenzo Villani.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Status.h"

static HTTP_StatusItem items[] = {
	[HTTP_Status_Info_Continue] = HTTP_StatusItem(
		100, "Continue", "The client should continue with its request"
	),

	[HTTP_Status_Info_SwitchingProtocol] = HTTP_StatusItem(
		101, "Switching Protocols", "The server is willing "
		"to change the application protocol being used on this connection"
	),

	[HTTP_Status_Info_Processing] = HTTP_StatusItem(
		102, "Processing", "Interim response used to inform the "
		"client that the server has accepted the complete request, but has not yet completed it"
	),

	[HTTP_Status_Success_Ok] = HTTP_StatusItem(
		200, "OK", "The request has succeeded"
	),

	[HTTP_Status_Success_Created] = HTTP_StatusItem(
		201, "Created", "The request has been fulfilled and "
		"resulted in a new resource being created"
	),

	[HTTP_Status_Success_Accepted] = HTTP_StatusItem(
		202, "Accepted", "The request has been accepted for "
		"processing, but the processing has not been completed"
	),

	[HTTP_Status_Success_NonAuthoritative] = HTTP_StatusItem(
		203, "Non-Authoritative Information", "The "
		"returned metainformation is not the definitive set as available from the origin server"
	),

	[HTTP_Status_Success_NoContent] = HTTP_StatusItem(
		204, "No Content", "The server has fulfilled the request "
		"but does not need to return an entity-body, and might want to return "
		"updated metainformation"
	),

	[HTTP_Status_Success_ResetContent] = HTTP_StatusItem(
		205, "Reset Content", "The server has fulfilled the "
		"request and the user agent should reset the document view which caused the request "
		"to be sent"
	),

	[HTTP_Status_Success_PartialContent] = HTTP_StatusItem(
		206, "Partial Content", "The server has fulfilled "
		"the partial get request for the resource"
	),

	[HTTP_Status_Success_MultiStatus] = HTTP_StatusItem(
		207, "Multi-Status", "Provides status for multiple "
		"independent operations"
	),

	[HTTP_Status_Redirection_MultipleChoices] = HTTP_StatusItem(
		300, "Multiple Choices", "The requested "
		"resource corresponds to any one of a set of representations"
	),

	[HTTP_Status_Redirection_Permanent] = HTTP_StatusItem(
		301, "Moved Permanently", "The requested resource has "
		"been assigned a new permanent URI"
	),

	[HTTP_Status_Redirection_Found] = HTTP_StatusItem(
		302, "Found", "The requested resource can be found under "
		"a different URI"
	),

	[HTTP_Status_Redirection_SeeOther] = HTTP_StatusItem(
		303, "See Other", "The response to the request can be "
		"found under a different URI"
	),

	[HTTP_Status_Redirection_NotModified] = HTTP_StatusItem(
		304, "Not Modified", "The client has performed a "
		"conditional GET request and the document has not been modified"
	),

	[HTTP_Status_Redirection_UseProxy] = HTTP_StatusItem(
		305, "Use Proxy", "The requested resource must be "
		"accessed through the proxy given by the location field"
	),

	[HTTP_Status_Redirection_Temporary] = HTTP_StatusItem(
		307, "Temporary Redirect", "The requested resource "
		"resides temporarily under a different URI"
	),

	[HTTP_Status_ClientError_BadRequest] = HTTP_StatusItem(
		400, "Bad Request", "The request could not be "
		"understood by the server due to malformed syntax"
	),

	[HTTP_Status_ClientError_Unauthorized] = HTTP_StatusItem(
		401, "Unauthorized", "The request requires user "
		"authentication"
	),

	[HTTP_Status_ClientError_PaymentRequired] = HTTP_StatusItem(
		402, "Payment Required", "This code is "
		"reserved for future use"
	),

	[HTTP_Status_ClientError_Forbidden] = HTTP_StatusItem(
		403, "Forbidden", "The server understood the "
		"request, but is refusing to fulfill it"
	),

	[HTTP_Status_ClientError_NotFound] = HTTP_StatusItem(
		404, "Not Found", "The server has not found anything "
		"matching the request URI"
	),

	[HTTP_Status_ClientError_MethodNotAllowed] = HTTP_StatusItem(
		405, "Method Not Allowed", "The method "
		"specified in the request is not allowed for the resource identified by the request URI"
	),

	[HTTP_Status_ClientError_NotAcceptable] = HTTP_StatusItem(
		406, "Not Acceptable", "The resource identified "
		"by the request is only capable of generating response entities which have content "
		"characteristics not acceptable according to the accept headers sent in the request"
	),

	[HTTP_Status_ClientError_ProxyAuthentificationRequired] = HTTP_StatusItem(
		407, "Proxy Authentication "
		"Required", "This code is similar to Unauthorized, but indicates that the client must first"
		" authenticate itself with the proxy"
	),

	[HTTP_Status_ClientError_RequestTimeout] = HTTP_StatusItem(
		408, "Request Timeout", "The client did not "
		"produce a request within the time that the server was prepared to wait"
	),

	[HTTP_Status_ClientError_Conflict] = HTTP_StatusItem(
		409, "Conflict", "The request could not be completed "
		"due to a conflict with the current state of the resource"
	),

	[HTTP_Status_ClientError_Gone] = HTTP_StatusItem(
		410, "Gone", "The requested resource is no longer "
		"available at the server and no forwarding address is known"
	),

	[HTTP_Status_ClientError_LengthRequired] = HTTP_StatusItem(
		411, "Length Required", "The server refuses "
		"to accept the request without a defined content length"
	),

	[HTTP_Status_ClientError_PreconditionFailed] = HTTP_StatusItem(
		412, "Precondition Failed", "The "
		"precondition given in one or more of the request header fields evaluated to false when "
		"it was tested on the server"
	),

	[HTTP_Status_ClientError_RequestEntityTooLarge] = HTTP_StatusItem(
		413, "Request Entity Too Large", "The "
		"server is refusing to process a request because the request entity is larger than the "
		"server is willing or able to process"
	),

	[HTTP_Status_ClientError_RequestUriTooLong] = HTTP_StatusItem(
		414, "Request URI Too Long", "The server "
		"is refusing to service the request because the request URI is longer than the server is"
		" willing to interpret"
	),

	[HTTP_Status_ClientError_UnsupportedMediaType] = HTTP_StatusItem(
		415, "Unsupported Media Type", "The "
		"server is refusing to service the request because the entity of the request is in a format"
		"not supported by the requested resource for the requested method"
	),

	[HTTP_Status_ClientError_RequestedRangeNotSatisfiable] = HTTP_StatusItem(
		416, "Requested Range Not "
		"Satisfiable", "For byte ranges, this means that the first byte position were greater "
		"than the current length of the selected resource"
	),

	[HTTP_Status_ClientError_ExpectationFailed] = HTTP_StatusItem(
		417, "Expectation Failed", "The expectation "
		"given in the request header could not be met by this server"
	),

	[HTTP_Status_ClientError_UnprocessableEntity] = HTTP_StatusItem(
		422, "Unprocessable Entity", "The server "
		"understands the content type of the request entity and the syntax of the request entity "
		"is correct but was unable to process the contained instructions"
	),

	[HTTP_Status_ClientError_Locked] = HTTP_StatusItem(
		423, "Locked", "The source or destination resource of "
		"a method is locked"
	),

	[HTTP_Status_ClientError_FailedDependency] = HTTP_StatusItem(
		424, "Failed Dependency", "The method could "
		"not be performed on the resource because the requested action depended on another action "
		"and that action failed"
	),

	[HTTP_Status_ServerError_Internal] = HTTP_StatusItem(
		500, "Internal Server Error", "The server encountered "
		"an unexpected condition which prevented it from fulfilling the request"
	),

	[HTTP_Status_ServerError_NotImplemented] = HTTP_StatusItem(
		501, "Not Implemented", "The server does not "
		"support the functionality required to fulfill the request"
	),

	[HTTP_Status_ServerError_BadGateway] = HTTP_StatusItem(
		502, "Bad Gateway", "The server, while acting as "
		"a gateway or proxy, received an invalid response from the upstream server it accessed in "
		"attempting to fulfill the request"
	),

	[HTTP_Status_ServerError_ServiceUnavailable] = HTTP_StatusItem(
		503, "Service Unavailable", "The server "
		"is currently unable to handle the request due to a temporary overloading or maintenance "
		"of the server"
	),

	[HTTP_Status_ServerError_GatewayTimeout] = HTTP_StatusItem(
		504, "Gateway Timeout", "The server, while "
		"acting as a gateway or proxy, did not receive a timely response from the upstream server "
		"specified by the URI (e.g. HTTP, FTP, LDAP) or some other auxiliary server (e.g. DNS) "
		"it needed to access in attempting to complete the request"
	),

	[HTTP_Status_ServerError_VersionNotSupported] = HTTP_StatusItem(
		505, "Version Not Supported", "The server"
		" does not support, or refuses to support, the protocol version that was used in the"
		"request message"
	),

	[HTTP_Status_ServerError_InsufficientStorage] = HTTP_StatusItem(
		507, "Insufficient Storage", "The method "
		"could not be performed on the resource because the server is unable to store the "
		"representation needed to successfully complete the request"
	)
};

HTTP_StatusItem HTTP_Status_GetItem(HTTP_Status status) {
	return items[status];
}

HTTP_Status HTTP_Status_GetStatusByCode(int code) {
	for (HTTP_Status status = 0; status < HTTP_Status_Length; status++) {
		if (code == items[status].code) {
			return status;
		}
	}

	return HTTP_Status_Unset;
}
