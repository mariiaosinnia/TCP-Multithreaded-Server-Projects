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
		std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
		closesocket(server_socket);
		WSACleanup();
		return;
	}

	thread_pool.add_task([client_socket, this]() {
		processRequest(client_socket);
	});
	
}

void Server::processRequest(SOCKET client_socket) {
	while (true) {
		uint32_t net_header_size = 0;
		if (!recvAll(client_socket, reinterpret_cast<char*>(&net_header_size), REQUEST_HEADER_SIZE_BYTES)) {
			return;
		}
		uint32_t header_size = ntohl(net_header_size);

		std::vector<char> buffer(header_size);
		if (!recvAll(client_socket, buffer.data(), header_size)) {
			return;
		}
		Request request = request_parser.parseRequest(buffer);
		std::string client_name = request.client_name;
		Command command = request.command;
		std::string file_name = request.file_name;
		uint32_t file_size = request.file_size;

		switch (command) {
		case Command::Get:
			command_handler.handleGet(client_socket, client_name, file_name);
			break;
		case Command::List:
			command_handler.handleList(client_socket, client_name);
			break;
		case Command::Put:
			command_handler.handlePut(client_socket, client_name, file_name, file_size);
			break;
		case Command::Delete:
			command_handler.handleDelete(client_socket, client_name, file_name);
			break;
		case Command::Info:
			command_handler.handleInfo(client_socket, client_name, file_name);
			break;
		default:
			command_handler.handleInvalid(client_socket);
		}
	}

	closesocket(client_socket);
}


void Server::cleanUp() {
	closesocket(server_socket);
	WSACleanup();
}

void Server::run() {
	initialize();
	createSocket();
	bindSocket();
	startListening();

	while (true)
	{
		acceptConnection();
	}

	cleanUp();
}