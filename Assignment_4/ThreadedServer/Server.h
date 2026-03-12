#pragma once
#include <WinSock2.h>
#include "RequestParser.h"
#include "CommandHandler.h"
#include "Protocol.h"
#include "ThreadPool.h"
#include <vector>
#pragma comment(lib, "ws2_32.lib")

class Server
{
private:
	int port = 12345;
	SOCKET server_socket;
	sockaddr_in server_addr;

	ThreadPool thread_pool;
	RequestParser request_parser;
	CommandHandler command_handler;

	void initialize();
	void createSocket();
	void bindSocket();
	void startListening();
	void acceptConnection();
	void processRequest(SOCKET client_socket);
	void cleanUp();

public:
	void run();
};

struct SocketRAII {
	SOCKET socket;
	SocketRAII(SOCKET sock) : socket(sock){}
	~SocketRAII() {
		if (socket != INVALID_SOCKET) {
			closesocket(socket);
		}
	}

	SocketRAII(const SocketRAII&) = delete;
	SocketRAII& operator=(const SocketRAII&) = delete;

	SocketRAII(SocketRAII&& other) noexcept : socket(other.socket) {
		other.socket = INVALID_SOCKET;
	}

	SocketRAII& operator=(SocketRAII&& other) noexcept {
		if (this != &other) {
			if (socket != INVALID_SOCKET) {
				closesocket(socket);
			}
			socket = other.socket;
			other.socket = INVALID_SOCKET;
		}
		return *this;
	}
};

