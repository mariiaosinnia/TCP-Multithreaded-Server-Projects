#include "Client.h"
#include <iostream>
#include <Ws2tcpip.h>
#include <fstream>
#include <algorithm>

void Client::initialize() {
	WSADATA wsa_data;
	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
		std::cerr << "WSAStartup failed" << std::endl;
		return;
	}
}

void Client::createSocket() {
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET) {
		std::cerr << "Error creating socket " << WSAGetLastError() << std::endl;
		WSACleanup();
		return;
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	InetPton(AF_INET, server_ip, &server_addr.sin_addr);
}

void Client::connectToServer() {
	if (connect(client_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR) {
		std::cerr << "Connect failed with error: " << WSAGetLastError() << std::endl;
		closesocket(client_socket);
		WSACleanup();
		return;
	}
}

bool Client::sendRequest(std::vector<char>& buffer) {
	uint32_t header_size = static_cast<uint32_t>(buffer.size());
	uint32_t net_header_size = htonl(header_size);
	if (!sendAll(client_socket, reinterpret_cast<char*>(&net_header_size), REQUEST_HEADER_SIZE_BYTES)) {
		return false;
	}

	if (!sendAll(client_socket, buffer.data(), header_size)) {
		return false;
	}
	return true;
}

void Client::cleanUp() {
	closesocket(client_socket);
	WSACleanup();
}

Status Client::receiveStatus()
{
	uint8_t byte;
	recv(client_socket, reinterpret_cast<char*>(&byte), STATUS_BYTES, 0);
	return static_cast<Status>(byte);
}

uint32_t Client::receivePayloadLength()
{
	uint32_t net_payload_length;
	recvAll(client_socket, reinterpret_cast<char*>(&net_payload_length), PAYLOAD_LENGTH_BYTES);
	return ntohl(net_payload_length);
}

void Client::run() {
	initialize();
	createSocket();
	connectToServer();
}

void Client::get(const std::string& client_name, const std::string& file_name) {
	std::vector<char> header = request_builder.buildGetRequest(client_name, file_name);

	if (!sendRequest(header)) {
		std::cout << "Failed to send request" << std::endl;
		return;
	}

	Status status = receiveStatus();
	if (status == Status::FILE_NOT_FOUND) {
		std::cout << "File not found" << std::endl;
		return;
	}
	std::ofstream file;
	file.open(file_directory + "/" + file_name, std::ios::app);

	uint32_t payload_size = receivePayloadLength();
	uint32_t total_received = 0;
	std::vector<char> buffer(CHUNK_SIZE);
	while (total_received < payload_size) {
		int to_read = std::min<int>(CHUNK_SIZE, payload_size - total_received);
		if (!recvAll(client_socket, buffer.data(), to_read)) {
			return;
		}

		file.write(buffer.data(), to_read);
		total_received += to_read;
	}
	std::cout << "File was successfully received" << std::endl;
}

void Client::list(const std::string& client_name) {
	std::vector<char> header = request_builder.buildListRequest(client_name);

	if (!sendRequest(header)) {
		std::cout << "Failed to send request" << std::endl;
		return;
	}

	Status status = receiveStatus();
	if (status != Status::SUCCESS) {
		std::cout << "Server error\n";
		return;
	}

	uint32_t payload_length = receivePayloadLength();
	std::vector<char> buffer(payload_length);

	if (!recvAll(client_socket, buffer.data(), payload_length)) {
		std::cout << "Failed to receive payload" << std::endl;
		return;
	}

	std::string result(buffer.begin(), buffer.end());
	std::cout << result << std::endl;
}

void Client::put(const std::string& client_name, const std::string& file_name) {
	std::ifstream file(file_directory + "/" + file_name, std::ios::binary);
	if (!file.is_open()) {
		std::cout << "File not found" << std::endl;
		return;
	}
	file.seekg(0, std::ios::end);
	uint32_t file_size = static_cast<uint32_t>(file.tellg());
	file.seekg(0, std::ios::beg);

	std::vector<char> header = request_builder.buildPutRequest(client_name, file_name, file_size);
	if (!sendRequest(header)) {
		std::cout << "Failed to send request" << std::endl;
		return;
	}

	std::vector<char> buffer(CHUNK_SIZE);
	int total_sent = 0;
	while (total_sent < file_size) {
		int to_sent = std::min<int>(CHUNK_SIZE, file_size - total_sent);
		file.read(buffer.data(), to_sent);
		if (!sendAll(client_socket, buffer.data(), to_sent)) {
			return;
		}
		total_sent += to_sent;
	}

	Status status = receiveStatus();
	if (status == Status::SUCCESS) {
		std::cout << "File was successfully uploaded to the server" << std::endl;
	}
	else {
		std::cout << "Upload failed" << std::endl;
	}
}

void Client::deleteFile(const std::string& client_name, const std::string& file_name) {
	std::vector<char> header = request_builder.buildDeleteRequest(client_name, file_name);
	if (!sendRequest(header)) {
		std::cout << "Failed to send request" << std::endl;
		return;
	}

	Status status = receiveStatus();
	if (status == Status::FILE_NOT_FOUND) {
		std::cout << "File not found" << std::endl;
		return;
	}
	if (status == Status::INTERNAL_ERROR) {
		std::cout << "Internal error occurred" << std::endl;
		return;
	}
	if (status == Status::SUCCESS) {
		std::cout << "File was successfully deleted" << std::endl;
	}
}

void Client::info(const std::string& client_name, const std::string& file_name) {
	std::vector<char> header = request_builder.buildInfoRequest(client_name, file_name);

	if (!sendRequest(header)) {
		std::cout << "Failed to send request" << std::endl;
		return;
	}

	Status status = receiveStatus();
	if (status == Status::FILE_NOT_FOUND) {
		std::cout << "File not found" << std::endl;
		return;
	}

	uint32_t payload_size = receivePayloadLength();
	std::cout << "File size: " << payload_size << "\n";
}

