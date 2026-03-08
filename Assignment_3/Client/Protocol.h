#pragma once
#include <string>
#include <vector>

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