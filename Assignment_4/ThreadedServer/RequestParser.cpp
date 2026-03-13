#include "RequestParser.h"
#include <WinSock2.h>

Request RequestParser::parseRequest(const std::vector<char>& buffer) {
	Request request;
	size_t offset = 0;
	
	uint16_t client_name_length = 0;
	memcpy(&client_name_length, buffer.data() + offset, CLIENT_NAME_LENGTH_BYTES);
	client_name_length = ntohs(client_name_length);
	offset += CLIENT_NAME_LENGTH_BYTES;

	request.client_name = std::string(buffer.data() + offset, client_name_length);
	offset += client_name_length;

	request.command = static_cast<Command>(buffer[offset]);
	offset += COMMAND_BYTES;

	switch (request.command) {
	case Command::Get:
	case Command::Put:
	case Command::Delete:
	case Command::Info:
	{
		uint16_t file_name_length = 0;
		memcpy(&file_name_length, buffer.data() + offset, FILENAME_LENGTH_BYTES);
		file_name_length = ntohs(file_name_length);
		offset += FILENAME_LENGTH_BYTES;

		request.file_name = std::string(buffer.data() + offset, file_name_length);
		offset += file_name_length;

		break;
	}
	case Command::List: break;
	}
	if (request.command == Command::Put) {
		uint32_t file_size = 0;
		memcpy(&file_size, buffer.data() + offset, FILE_SIZE_BYTES);
		file_size = ntohl(file_size);
		request.file_size = file_size;
	}

	return request;
}
