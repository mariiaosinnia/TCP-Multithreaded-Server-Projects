#include "CommandHandler.h"
#include "Protocol.h"
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>

bool CommandHandler::sendAll(SOCKET socket, char* data, int size){
	int total_sent = 0;
	while (total_sent < size) {
		int sent = send(socket, data + total_sent, size - total_sent, 0);
		if (sent <= 0){
			return false;
		}
		total_sent += sent;
	}
	return true;
}
bool CommandHandler::recvAll(SOCKET socket, char* buffer, int size) {
	int total_received = 0;
	while (total_received < size) {
		int bytes_received = recv(socket, buffer + total_received, size - total_received, 0);
		if (bytes_received <= 0) {
			return false;
		}
		total_received += bytes_received;
	}
	return true;
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
	if (!sendAll(client_socket, reinterpret_cast<char*>(&net_file_size), sizeof(net_file_size))) {
		return;
	}

	const size_t CHUNK_SIZE = 1024;
	std::vector<char> buffer(CHUNK_SIZE);

	while (file) {
		file.read(buffer.data(), CHUNK_SIZE);
		std::streamsize bytes_read = file.gcount();

		if (bytes_read > 0) {
			if (!sendAll(client_socket, buffer.data(), bytes_read)) {
				return;
		}
	}
	}
	file.close();
}

void CommandHandler::handleList(SOCKET client_socket){
	std::string result;
	try {
		for (const auto& entry : std::filesystem::directory_iterator("server_files")) {
			if (entry.is_regular_file()) {
				result += entry.path().filename().string();
				result += "\n";
			}
		}
	}
	catch (...) {
		uint8_t status = 1;
		send(client_socket, reinterpret_cast<char*>(&status), STATUS_BYTES, 0);
		return;
	}

	uint8_t status = 0;
	send(client_socket, reinterpret_cast<char*>(&status), STATUS_BYTES, 0);

	uint32_t payload_size = static_cast<uint32_t>(result.length());
	uint32_t net_payload_size = htonl(payload_size);
	if (!sendAll(client_socket, reinterpret_cast<char*>(&net_payload_size), PAYLOAD_LENGTH_BYTES)) {
		return;
	}

	if (!sendAll(client_socket, result.data(), payload_size)) {
		return;
	}
}

	sendAll(client_socket, result.data(), payload_size);
}
