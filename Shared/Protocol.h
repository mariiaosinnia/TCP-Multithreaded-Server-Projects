#pragma once
#include <string>
#include <vector>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

const int REQUEST_HEADER_SIZE_BYTES = 4;
const int CLIENT_NAME_LENGTH_BYTES = 2;
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
	std::string client_name;
	Command command;
	std::string file_name = "";
	uint32_t file_size = 0;
};

bool recvAll(SOCKET socket, char* buffer, int size);
bool sendAll(SOCKET socket, const char* buffer, int size);