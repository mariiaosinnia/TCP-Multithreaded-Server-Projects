#include "CommandHandler.h"
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>

void CommandHandler::sendStatus(SOCKET socket, Status status_enum){
	uint8_t status = static_cast<uint8_t>(status_enum);
	send(socket, reinterpret_cast<char*>(&status), STATUS_BYTES, 0);
}

void CommandHandler::handleGet(SOCKET client_socket, std::string& file_name){
	std::ifstream file(file_directory + "/" + file_name, std::ios::binary);

	if (!file.is_open()) {
		Status status = Status::FILE_NOT_FOUND;
		sendStatus(client_socket, status);
		return;
	}
	Status status = Status::SUCCESS;
	sendStatus(client_socket, status);

	file.seekg(0, std::ios::end);
	uint32_t file_size = static_cast<uint32_t>(file.tellg());
	file.seekg(0, std::ios::beg);

	uint32_t net_file_size = htonl(file_size);
	if (!sendAll(client_socket, reinterpret_cast<char*>(&net_file_size), sizeof(net_file_size))) {
		return;
	}

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
		Status status = Status::INTERNAL_ERROR;
		sendStatus(client_socket, status);
		return;
	}

	Status status = Status::SUCCESS;
	sendStatus(client_socket, status);

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
		Status status = Status::INTERNAL_ERROR;
		sendStatus(client_socket, status);
		return;
	}

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
	Status status = Status::SUCCESS;
	sendStatus(client_socket, status);
}

void CommandHandler::handleDelete(SOCKET client_socket, std::string& file_name){
	std::filesystem::path file_path = file_directory + "/" + file_name;
	try {
		if (std::filesystem::remove(file_path)) {
			Status status = Status::SUCCESS;
			sendStatus(client_socket, status);
		}
		else {
			Status status = Status::FILE_NOT_FOUND;
			sendStatus(client_socket, status);
		}
	}
	catch (...) {
		Status status = Status::INTERNAL_ERROR;
		sendStatus(client_socket, status);
	}
}

void CommandHandler::handleInfo(SOCKET client_socket, std::string& file_name){
	std::filesystem::path file_path = file_directory + "/" + file_name;
	if (!std::filesystem::exists(file_path) || !std::filesystem::is_regular_file(file_path)) {
		Status status = Status::FILE_NOT_FOUND;
		sendStatus(client_socket, status);
		return;
	}
	Status status = Status::SUCCESS;
	sendStatus(client_socket, status);

	uint32_t file_size = static_cast<uint32_t>(std::filesystem::file_size(file_path));
	uint32_t net_file_size = htonl(file_size);

	sendAll(client_socket, reinterpret_cast<char*>(&net_file_size), sizeof(net_file_size));
}

void CommandHandler::handleInvalid(SOCKET client_socket){
	Status status = Status::INVALID_REQUEST;
	sendStatus(client_socket, status);
}
