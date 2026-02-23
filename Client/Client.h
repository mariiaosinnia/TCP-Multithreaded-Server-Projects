#pragma once
#include <WinSock2.h>
#include <vector>
#include "Protocol.h"
#include "RequestBuilder.h"

#pragma comment(lib, "ws2_32.lib")

class Client{
private:
	int port = 12345;
	SOCKET client_socket;
	PCWSTR server_ip = L"127.0.0.1";
	sockaddr_in server_addr;

	RequestBuilder request_builder;

	std::string file_directory = "client_files";

	void initialize();
	void createSocket();
	void connectToServer();
	bool sendRequest(std::vector<char>& buffer);
	void cleanUp();

	bool recvAll(char* buffer, int size);
	bool sendAll(char* buffer, int size);

	Status receiveStatus();
	uint32_t receivePayloadLength();
public:
	void run();
	void get(const std::string& file_name);
	void list();
};

