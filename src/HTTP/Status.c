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

#import "Status.h"

static ref(Item) items[] = {
	[ref(Info_Continue)] = scall(Item,
		100, "Continue", "The client should continue with its request"
	),

	[ref(Info_SwitchingProtocol)] = scall(Item,
		101, "Switching Protocols", "The server is willing "
		"to change the application protocol being used on this connection"
	),

	[ref(Info_Processing)] = scall(Item,
		102, "Processing", "Interim response used to inform the "
		"client that the server has accepted the complete request, but has not yet completed it"
	),

	[ref(Success_Ok)] = scall(Item,
		200, "OK", "The request has succeeded"
	),

	[ref(Success_Created)] = scall(Item,
		201, "Created", "The request has been fulfilled and "
		"resulted in a new resource being created"
	),

	[ref(Success_Accepted)] = scall(Item,
		202, "Accepted", "The request has been accepted for "
		"processing, but the processing has not been completed"
	),

	[ref(Success_NonAuthoritative)] = scall(Item,
		203, "Non-Authoritative Information", "The "
		"returned metainformation is not the definitive set as available from the origin server"
	),

	[ref(Success_NoContent)] = scall(Item,
		204, "No Content", "The server has fulfilled the request "
		"but does not need to return an entity-body, and might want to return "
		"updated metainformation"
	),

	[ref(Success_ResetContent)] = scall(Item,
		205, "Reset Content", "The server has fulfilled the "
		"request and the user agent should reset the document view which caused the request "
		"to be sent"
	),

	[ref(Success_PartialContent)] = scall(Item,
		206, "Partial Content", "The server has fulfilled "
		"the partial get request for the resource"
	),

	[ref(Success_MultiStatus)] = scall(Item,
		207, "Multi-Status", "Provides status for multiple "
		"independent operations"
	),

	[ref(Redirection_MultipleChoices)] = scall(Item,
		300, "Multiple Choices", "The requested "
		"resource corresponds to any one of a set of representations"
	),

	[ref(Redirection_Permanent)] = scall(Item,
		301, "Moved Permanently", "The requested resource has "
		"been assigned a new permanent URI"
	),

	[ref(Redirection_Found)] = scall(Item,
		302, "Found", "The requested resource can be found under "
		"a different URI"
	),

	[ref(Redirection_SeeOther)] = scall(Item,
		303, "See Other", "The response to the request can be "
		"found under a different URI"
	),

	[ref(Redirection_NotModified)] = scall(Item,
		304, "Not Modified", "The client has performed a "
		"conditional GET request and the document has not been modified"
	),

	[ref(Redirection_UseProxy)] = scall(Item,
		305, "Use Proxy", "The requested resource must be "
		"accessed through the proxy given by the location field"
	),

	[ref(Redirection_Temporary)] = scall(Item,
		307, "Temporary Redirect", "The requested resource "
		"resides temporarily under a different URI"
	),

	[ref(ClientError_BadRequest)] = scall(Item,
		400, "Bad Request", "The request could not be "
		"understood by the server due to malformed syntax"
	),

	[ref(ClientError_Unauthorized)] = scall(Item,
		401, "Unauthorized", "The request requires user "
		"authentication"
	),

	[ref(ClientError_PaymentRequired)] = scall(Item,
		402, "Payment Required", "This code is "
		"reserved for future use"
	),

	[ref(ClientError_Forbidden)] = scall(Item,
		403, "Forbidden", "The server understood the "
		"request, but is refusing to fulfill it"
	),

	[ref(ClientError_NotFound)] = scall(Item,
		404, "Not Found", "The server has not found anything "
		"matching the request URI"
	),

	[ref(ClientError_MethodNotAllowed)] = scall(Item,
		405, "Method Not Allowed", "The method "
		"specified in the request is not allowed for the resource identified by the request URI"
	),

	[ref(ClientError_NotAcceptable)] = scall(Item,
		406, "Not Acceptable", "The resource identified "
		"by the request is only capable of generating response entities which have content "
		"characteristics not acceptable according to the accept headers sent in the request"
	),

	[ref(ClientError_ProxyAuthentificationRequired)] = scall(Item,
		407, "Proxy Authentication "
		"Required", "This code is similar to Unauthorized, but indicates that the client must first"
		" authenticate itself with the proxy"
	),

	[ref(ClientError_RequestTimeout)] = scall(Item,
		408, "Request Timeout", "The client did not "
		"produce a request within the time that the server was prepared to wait"
	),

	[ref(ClientError_Conflict)] = scall(Item,
		409, "Conflict", "The request could not be completed "
		"due to a conflict with the current state of the resource"
	),

	[ref(ClientError_Gone)] = scall(Item,
		410, "Gone", "The requested resource is no longer "
		"available at the server and no forwarding address is known"
	),

	[ref(ClientError_LengthRequired)] = scall(Item,
		411, "Length Required", "The server refuses "
		"to accept the request without a defined content length"
	),

	[ref(ClientError_PreconditionFailed)] = scall(Item,
		412, "Precondition Failed", "The "
		"precondition given in one or more of the request header fields evaluated to false when "
		"it was tested on the server"
	),

	[ref(ClientError_RequestEntityTooLarge)] = scall(Item,
		413, "Request Entity Too Large", "The "
		"server is refusing to process a request because the request entity is larger than the "
		"server is willing or able to process"
	),

	[ref(ClientError_RequestUriTooLong)] = scall(Item,
		414, "Request URI Too Long", "The server "
		"is refusing to service the request because the request URI is longer than the server is"
		" willing to interpret"
	),

	[ref(ClientError_UnsupportedMediaType)] = scall(Item,
		415, "Unsupported Media Type", "The "
		"server is refusing to service the request because the entity of the request is in a format"
		"not supported by the requested resource for the requested method"
	),

	[ref(ClientError_RequestedRangeNotSatisfiable)] = scall(Item,
		416, "Requested Range Not "
		"Satisfiable", "For byte ranges, this means that the first byte position were greater "
		"than the current length of the selected resource"
	),

	[ref(ClientError_ExpectationFailed)] = scall(Item,
		417, "Expectation Failed", "The expectation "
		"given in the request header could not be met by this server"
	),

	[ref(ClientError_UnprocessableEntity)] = scall(Item,
		422, "Unprocessable Entity", "The server "
		"understands the content type of the request entity and the syntax of the request entity "
		"is correct but was unable to process the contained instructions"
	),

	[ref(ClientError_Locked)] = scall(Item,
		423, "Locked", "The source or destination resource of "
		"a method is locked"
	),

	[ref(ClientError_FailedDependency)] = scall(Item,
		424, "Failed Dependency", "The method could "
		"not be performed on the resource because the requested action depended on another action "
		"and that action failed"
	),

	[ref(ServerError_Internal)] = scall(Item,
		500, "Internal Server Error", "The server encountered "
		"an unexpected condition which prevented it from fulfilling the request"
	),

	[ref(ServerError_NotImplemented)] = scall(Item,
		501, "Not Implemented", "The server does not "
		"support the functionality required to fulfill the request"
	),

	[ref(ServerError_BadGateway)] = scall(Item,
		502, "Bad Gateway", "The server, while acting as "
		"a gateway or proxy, received an invalid response from the upstream server it accessed in "
		"attempting to fulfill the request"
	),

	[ref(ServerError_ServiceUnavailable)] = scall(Item,
		503, "Service Unavailable", "The server "
		"is currently unable to handle the request due to a temporary overloading or maintenance "
		"of the server"
	),

	[ref(ServerError_GatewayTimeout)] = scall(Item,
		504, "Gateway Timeout", "The server, while "
		"acting as a gateway or proxy, did not receive a timely response from the upstream server "
		"specified by the URI (e.g. HTTP, FTP, LDAP) or some other auxiliary server (e.g. DNS) "
		"it needed to access in attempting to complete the request"
	),

	[ref(ServerError_VersionNotSupported)] = scall(Item,
		505, "Version Not Supported", "The server"
		" does not support, or refuses to support, the protocol version that was used in the"
		"request message"
	),

	[ref(ServerError_InsufficientStorage)] = scall(Item,
		507, "Insufficient Storage", "The method "
		"could not be performed on the resource because the server is unable to store the "
		"representation needed to successfully complete the request"
	)
};

sdef(ref(Item), GetItem, self status) {
	return items[status];
}

sdef(self, GetStatusByCode, u16 code) {
	for (self status = 0; status < ref(Length); status++) {
		if (code == items[status].code) {
			return status;
		}
	}

	return ref(Unset);
}
