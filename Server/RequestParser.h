#pragma once
#include "Protocol.h"
#include <vector>

class RequestParser{
public:
	Request parseRequest(const std::vector<char>& buffer);
};

