#pragma once
#include <WinSock2.h>
#include <string>

class CommandHandler{
private:
	bool sendAll(SOCKET socket, char* data, int size);
public:
	void handleGet(SOCKET client_socket, std::string& file_name);
	void handleList(SOCKET client_socket);
	void handlePut(SOCKET client_socket, std::string& file_name, uint32_t file_size);
	void handleDelete(SOCKET client_socket, std::string& file_name);
	void handleInfo(SOCKET client_socket, std::string& file_name);
};

