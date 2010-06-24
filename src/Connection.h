#ifndef CONNECTION_H
#define CONNECTION_H

#include "Client.h"
#include "DoublyLinkedList.h"

#define Connection_Define(name) \
	Client *client;             \
	DoublyLinkedList_DeclareRef(name)

typedef struct _Connection {
	Connection_Define(_Connection);
} Connection;

DoublyLinkedList_DeclareList(Connection, Connections);

#endif
