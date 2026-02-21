#pragma once
#include <vector>
#include <string>
class RequestBuilder{
private:
public:
    std::vector<char> buildGetRequest(const std::string& file_name);
    std::vector<char> buildPutRequest(const std::string& file_name, uint32_t file_size);
    std::vector<char> buildListRequest();
    std::vector<char> buildDeleteRequest(const std::string& file_name);
    std::vector<char> buildInfoRequest(const std::string& file_name);
};

