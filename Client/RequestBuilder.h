#pragma once
#include <vector>
#include <string>
#include "Protocol.h"

class RequestBuilder{
private:
    std::vector<char> serializeCommand(Command command);
    std::vector<char> serializeFileName(const std::string& file_name);
    std::vector<char> serializeFileSize(uint32_t file_size);
public:
    std::vector<char> buildGetRequest(const std::string& file_name);
    std::vector<char> buildListRequest();
    std::vector<char> buildPutRequest(const std::string& file_name, uint32_t file_size);
    std::vector<char> buildDeleteRequest(const std::string& file_name);
    std::vector<char> buildInfoRequest(const std::string& file_name);
};

