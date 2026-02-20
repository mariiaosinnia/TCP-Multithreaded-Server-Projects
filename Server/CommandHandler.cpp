#include "CommandHandler.h"
#include "Protocol.h"
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>

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
		for (const auto& entry : std::filesystem::directory_iterator(file_directory)) {
			if (entry.is_regular_file()) {
				result += entry.path().filename().string();
				result += "\n";
			}
		}
	}
	catch (...) {
		uint8_t status = 3;
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

void CommandHandler::handlePut(SOCKET client_socket, std::string& file_name, uint32_t file_size) {
	std::ofstream file(file_directory + "/" + file_name, std::ios::binary);
	if (!file.is_open()) {
		uint8_t status = 1;
		send(client_socket, reinterpret_cast<char*>(&status), STATUS_BYTES, 0);
	}
	const size_t CHUNK_SIZE = 1024;
	std::vector<char> buffer(CHUNK_SIZE);

	int total_received = 0;
	while (total_received < file_size) {
		uint32_t bytes_to_read = std::min<uint32_t>(CHUNK_SIZE, file_size - total_received);
		if (!recvAll(client_socket, buffer.data(), bytes_to_read)) {
			return;
		}

		file.write(buffer.data(), bytes_to_read);
		total_received += bytes_to_read;
	}
	uint8_t status = 0;
	send(client_socket, reinterpret_cast<char*>(&status), STATUS_BYTES, 0);
}

void CommandHandler::handleDelete(SOCKET client_socket, std::string& file_name){
	std::filesystem::path file_path = file_directory + "/" + file_name;
	try {
		if (std::filesystem::remove(file_path)) {
			uint8_t status = 0;
			send(client_socket, reinterpret_cast<char*>(&status), STATUS_BYTES, 0);
		}
		else {
			uint8_t status = 1;
			send(client_socket, reinterpret_cast<char*>(&status), STATUS_BYTES, 0);
		}
	}
	catch (...) {
		uint8_t status = 3;
		send(client_socket, reinterpret_cast<char*>(&status), STATUS_BYTES, 0);
	}
}

void CommandHandler::handleInfo(SOCKET client_socket, std::string& file_name){
	std::filesystem::path file_path = file_directory + "/" + file_name;
	if (!std::filesystem::exists(file_path) || !std::filesystem::is_regular_file(file_path)) {
		uint8_t status = static_cast<uint8_t>(Status::FILE_NOT_FOUND);
		send(client_socket, reinterpret_cast<char*>(&status), STATUS_BYTES, 0);
	}
	uint8_t status = static_cast<uint8_t>(Status::SUCCESS);
	send(client_socket, reinterpret_cast<char*>(&status), STATUS_BYTES);

	uint32_t file_size = static_cast<uint32_t>(std::filesystem::file_size(file_path));
	uint32_t net_file_size = htonl(file_size);

	sendAll(client_socket, reinterpret_cast<char*>(&net_file_size), sizeof(net_file_size));
}
