#pragma once
#include <string>
#include <vector>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

const int REQUEST_HEADER_SIZE_BYTES = 4;
const int COMMAND_BYTES = 1;
const int FILENAME_LENGTH_BYTES = 2;
const int FILE_SIZE_BYTES = 4;

const int STATUS_BYTES = 1;
const int PAYLOAD_LENGTH_BYTES = 4;

const int CHUNK_SIZE = 1024;

enum class Command : uint8_t {
	Get,
	List,
	Put,
	Delete,
	Info,
	Unknown
};

enum class Status : uint8_t {
	SUCCESS,
	FILE_NOT_FOUND,
	INVALID_REQUEST,
	INTERNAL_ERROR
};


struct Request {
	Command command;
	std::string file_name = "";
	uint32_t file_size = 0;
};

bool recvAll(SOCKET socket, char* buffer, int size) {
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

bool sendAll(SOCKET socket, const char* buffer, int size) {
	int total_sent = 0;
	while (total_sent < size) {
		int bytes_sent = send(socket, buffer + total_sent, size - total_sent, 0);
		if (bytes_sent <= 0) {
			return false;
		}
		total_sent += bytes_sent;
	}
	return true;
}