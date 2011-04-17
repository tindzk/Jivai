/*
 * Usage:
 * ------
 *
 * Start several telnet clients:
 * $ telnet localhost 1337
 *
 * Then type "info", "active" or "exit".
 */

#import <Main.h>
#import <Server.h>
#import <Socket.h>
#import <Logger.h>
#import <Integer.h>

size_t activeConn = 0;

// ----------------
// CustomConnection
// ----------------

#define self CustomConnection

class {
	size_t            id;
	Logger            *logger;
	Connection_Client *client;
};

def(void, Init, Connection_Client *client, Logger *logger) {
	this->id = activeConn;
	activeConn++;

	this->client = client;
	this->logger = logger;

	String strAddr = NetworkAddress_ToString(client->conn->addr);
	String strPort = Integer_ToString(client->conn->addr.port);
	String strFd   = Integer_ToString(client->conn->fd);

	Logger_Info(this->logger,
		$("Incoming TCP connection from %:%, fd=%"),
		strAddr.rd, strPort.rd, strFd.rd);

	String_Destroy(&strAddr);
	String_Destroy(&strPort);
	String_Destroy(&strFd);

	SocketConnection_Write(client->conn, $("Hi.\n"));
}

def(void, Destroy) {
	String strAddr = NetworkAddress_ToString(this->client->conn->addr);
	String strPort = Integer_ToString(this->client->conn->addr.port);
	String strFd   = Integer_ToString(this->client->conn->fd);

	Logger_Info(this->logger,
		$("Client %:%, fd=% disconnected"),
		strAddr.rd, strPort.rd, strFd.rd);

	String_Destroy(&strAddr);
	String_Destroy(&strPort);
	String_Destroy(&strFd);

	activeConn--;
}

def(Connection_Status, OnData) {
	String s = String_New(1024);
	s.len = SocketConnection_Read(this->client->conn, s.buf, String_GetSize(s));

	RdString input = String_Trim(s.rd);

	String strFd = Integer_ToString(this->client->conn->fd);
	Logger_Info(this->logger,
		$("Received data from fd=%: '%'"), strFd.rd, input);
	String_Destroy(&strFd);

	if (String_Equals(input, $("info"))) {
		String strId = Integer_ToString(this->id);

		String resp = String_Format($("You have the ID %.\n"), strId.rd);
		SocketConnection_Write(this->client->conn, resp.rd);

		String_Destroy(&resp);
		String_Destroy(&strId);
	} else if (String_Equals(input, $("active"))) {
		String strActive = Integer_ToString(activeConn);

		String resp = String_Format( $("% active connection(s).\n"),
			strActive.rd);

		SocketConnection_Write(this->client->conn, resp.rd);

		String_Destroy(&resp);
		String_Destroy(&strActive);
	} else if (String_Equals(input, $("exit"))) {
		SocketConnection_Write(this->client->conn, $("Bye.\n"));

		String_Destroy(&s);
		return Connection_Status_Close;
	}

	String_Destroy(&s);

	return Connection_Status_Open;
}

def(Connection_Status, OnPush) {
	return Connection_Status_Open;
}

Impl(Connection) = {
	.size    = sizeof(self),
	.init    = ref(Init),
	.destroy = ref(Destroy),
	.pull    = ref(OnData),
	.push    = ref(OnPush)
};

ExportImpl(Connection);

#undef self

// -----------
// Application
// -----------

#define self Application

def(bool, Run) {
	Server server = Server_New(CustomConnection_GetImpl(), &this->logger);;
	Server_SetEdgeTriggered(&server, false);
	Server_Listen(&server, 1337);

	try {
		Logger_Info(&this->logger, $("Server started."));
		EventLoop_Run(EventLoop_GetInstance());
	} catch (Signal, SigInt) {
		Logger_Info(&this->logger, $("Server shutdown."));
	} finally {
		Server_Destroy(&server);
	} tryEnd;

	return true;
}
