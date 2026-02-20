#include "CommandHandler.h"
#include "Protocol.h"
#include <fstream>

void CommandHandler::sendAll(SOCKET socket, char* data, int size){
	int total_sent = 0;
	while (total_sent < size) {
		int sent = send(socket, data + total_sent, size - total_sent, 0);
		if (sent <= 0){
			return;
		}
		total_sent += sent;
	}
}

void CommandHandler::handleGet(SOCKET client_socket, std::string& file_name){
	std::ifstream file(file_name, std::ios::binary);

	if (!file.is_open()) {
		uint8_t status = 1;
		send(client_socket, reinterpret_cast<char*>(&status), STATUS_BYTES, 0);
		return;
	}
	uint8_t status = 0;
	send(client_socket, reinterpret_cast<char*>(&status), STATUS_BYTES, 0);

	file.seekg(0, std::ios::end);
	uint32_t file_size = static_cast<uint32_t>(file.tellg());
	file.seekg(0, std::ios::beg);

	uint32_t net_file_size = htonl(file_size);
	sendAll(client_socket, reinterpret_cast<char*>(&net_file_size), sizeof(net_file_size));

	const size_t CHUNK_SIZE = 1024;
	std::vector<char> buffer(CHUNK_SIZE);

	while (file) {
		file.read(buffer.data(), CHUNK_SIZE);
		std::streamsize bytes_read = file.gcount();

		if (bytes_read > 0) {
			sendAll(client_socket, buffer.data(), bytes_read);
		}
	}
	file.close();
}
