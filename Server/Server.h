#pragma once
#include <WinSock2.h>
#include "RequestParser.h"
#pragma comment(lib, "ws2_32.lib")

class Server
{
private:
	int port = 12345;
	SOCKET server_socket;
	sockaddr_in server_addr;

	SOCKET client_socket;

	RequestParser request_parser;
	//ClientSession (???)
	//CommandHandler
	//ResponseBuilder
	//main or some class for client-server app
public:
	void initialize();
	void createSocket();
	void bindSocket();
	void startListening();
	void acceptConnection();
	void processRequest();
	void cleanUp();
};


