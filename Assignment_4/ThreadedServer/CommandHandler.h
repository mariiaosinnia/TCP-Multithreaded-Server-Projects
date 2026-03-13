#pragma once
#include <WinSock2.h>
#include <string>
#include "Protocol.h"

class CommandHandler {
private:
	void sendStatus(SOCKET socket, Status status_enum);
	std::string file_directory = "server_files";
public:
	void handleGet(SOCKET client_socket, std::string& client_name, std::string& file_name);
	void handleList(SOCKET client_socket, std::string& client_name);
	void handlePut(SOCKET client_socket, std::string& client_name, std::string& file_name, uint32_t file_size);
	void handleDelete(SOCKET client_socket, std::string& client_name, std::string& file_name);
	void handleInfo(SOCKET client_socket, std::string& client_name, std::string& file_name);
	void handleInvalid(SOCKET client_socket);
};

