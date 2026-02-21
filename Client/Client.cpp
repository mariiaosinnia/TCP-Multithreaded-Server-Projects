#include "Client.h"
#include <iostream>
#include <Ws2tcpip.h>
#include "Protocol.h"

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

void Client::connectToServer(){
	if (connect(client_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR) {
		std::cerr << "Connect failed with error: " << WSAGetLastError() << std::endl;
		closesocket(client_socket);
		WSACleanup();
		return;
	}
}

bool Client::sendRequest(std::vector<char>& buffer){
	uint32_t header_size = static_cast<uint32_t>(buffer.size());
	uint32_t net_header_size = htonl(header_size);
	if (!sendAll(reinterpret_cast<char*>(net_header_size), REQUEST_HEADER_SIZE_BYTES)) {
		return false;
	}

	if (!sendAll(buffer.data(), header_size)) {
		return false;
	}
	return true;
}

void Client::cleanUp() {
	closesocket(client_socket);
	WSACleanup();
}

bool Client::sendAll(char* buffer, int size) {
	int total_sent = 0;
	while (total_sent < size) {
		int bytes_sent = send(client_socket, buffer + total_sent, size - total_sent, 0);
		if (bytes_sent <= 0) {
			return false;
		}
		total_sent += bytes_sent;
	}
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
	recvAll(reinterpret_cast<char*>(&net_payload_length), PAYLOAD_LENGTH_BYTES);
	return ntohl(net_payload_length);
}

