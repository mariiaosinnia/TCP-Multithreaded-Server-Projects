#pragma once
#include <WinSock2.h>
#include <vector>
#pragma comment(lib, "ws2_32.lib")

class Client{
private:
	int port = 12345;
	SOCKET client_socket;
	PCWSTR server_ip = L"127.0.0.1";
	sockaddr_in server_addr;

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
	void get(std::vector<char>& file_name);
};

