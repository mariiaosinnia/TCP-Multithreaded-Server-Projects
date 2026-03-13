#pragma once
#include "Protocol.h"
#include <vector>

class RequestParser {
private:
	uint8_t has_name = 0xFF;
	std::string defaulte_dir = "default";
public:
	Request parseRequest(const std::vector<char>& buffer);
};

