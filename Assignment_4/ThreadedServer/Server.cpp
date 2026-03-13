#include "Server.h"
#include <iostream>

void Server::initialize() {
	WSADATA wsa_data;
	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
		std::cerr << "WSAStartup failed" << std::endl;
		return;
	}
}

void Server::createSocket() {
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == INVALID_SOCKET) {
		std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return;
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);
}

void Server::bindSocket() {
	if (bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr))) {
		std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
		closesocket(server_socket);
		WSACleanup();
		return;
	}
}

void Server::startListening() {
	thread_pool.initialize(3);
	is_running = true;

	if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
		closesocket(server_socket);
		WSACleanup();
		return;
	}
	std::cout << "Server is listening on a port: " << port << std::endl;
}

void Server::acceptConnection() {
	SOCKET client_socket = accept(server_socket, nullptr, nullptr);
	if (client_socket == SOCKET_ERROR) {
		if (!is_running) {
			return;
		}
		std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
		return;
	}

	total_clients++;

	thread_pool.add_task([client_socket, this]() {
		processRequest(client_socket);
	});
	
}

void Server::processRequest(SOCKET client_socket) {
	while (is_running) {
		uint32_t net_header_size = 0;
		if (!recvAll(client_socket, reinterpret_cast<char*>(&net_header_size), REQUEST_HEADER_SIZE_BYTES)) {
			break;
		}
		uint32_t header_size = ntohl(net_header_size);

		std::vector<char> buffer(header_size);
		if (!recvAll(client_socket, buffer.data(), header_size)) {
			break;
		}
		Request request = request_parser.parseRequest(buffer);
		std::string client_name = request.client_name;
		Command command = request.command;
		std::string file_name = request.file_name;
		uint32_t file_size = request.file_size;

		switch (command) {
		case Command::Get:
			get_requests++;
			command_handler.handleGet(client_socket, client_name, file_name);
			break;
		case Command::List:
			list_requests++;
			command_handler.handleList(client_socket, client_name);
			break;
		case Command::Put:
			put_requests++;
			command_handler.handlePut(client_socket, client_name, file_name, file_size);
			break;
		case Command::Delete:
			delete_requests++;
			command_handler.handleDelete(client_socket, client_name, file_name);
			break;
		case Command::Info:
			info_requests++;
			command_handler.handleInfo(client_socket, client_name, file_name);
			break;
		default:
			command_handler.handleInvalid(client_socket);
		}

		total_requests++;
	}

	closesocket(client_socket);
}

void Server::console_loop() {
	std::string command;

	while (is_running) {
		std::getline(std::cin, command);

		if (command == "exit") {
			std::cout << "Shutting down server..." << std::endl;

			is_running = false;
			closesocket(server_socket);
			break;
		}
	}
}


void Server::cleanUp() {
	thread_pool.terminate();
	closesocket(server_socket);
	WSACleanup();
	std::cout << "Server stopped" << std::endl;

	std::cout << "\n=== SERVER STATISTICS ===\n";

	std::cout << "Clients connected: " << total_clients << std::endl;
	std::cout << "Total requests: " << total_requests << std::endl;

	std::cout << "GET: " << get_requests << std::endl;
	std::cout << "PUT: " << put_requests << std::endl;
	std::cout << "LIST: " << list_requests << std::endl;
	std::cout << "DELETE: " << delete_requests << std::endl;
	std::cout << "INFO: " << info_requests << std::endl;

	std::cout << "====================\n";
}

void Server::run() {
	initialize();
	createSocket();
	bindSocket();
	startListening();

	std::thread console_thread(&Server::console_loop, this);

	while (is_running)
	{
		acceptConnection();
	}

	console_thread.join();

	cleanUp();
}