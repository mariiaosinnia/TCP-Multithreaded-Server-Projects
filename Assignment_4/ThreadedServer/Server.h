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

	std::atomic<bool> is_running = false;

	void initialize();
	void createSocket();
	void bindSocket();
	void startListening();
	void acceptConnection();
	void processRequest(SOCKET client_socket);
	void console_loop();
	void cleanUp();


	std::atomic<int> total_clients = 0;
	std::atomic<int> total_requests = 0;

	std::atomic<int> get_requests = 0;
	std::atomic<int> put_requests = 0;
	std::atomic<int> list_requests = 0;
	std::atomic<int> delete_requests = 0;
	std::atomic<int> info_requests = 0;
public:
	void run();
};

