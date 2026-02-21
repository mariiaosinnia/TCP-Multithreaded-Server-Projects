#include "RequestBuilder.h"
#include "Protocol.h"
#include <WinSock2.h>

std::vector<char> RequestBuilder::buildGetRequest(const std::string& file_name){
    std::vector<char> request_header;
    request_header.push_back(static_cast<uint8_t>(Command::Get));
    
    uint16_t file_name_length = static_cast<uint16_t>(file_name.length());
    uint16_t net_file_name_length = htons(file_name_length);
    request_header.insert(request_header.end(), reinterpret_cast<char*>(&net_file_name_length),
        reinterpret_cast<char*>(&net_file_name_length) + FILENAME_LENGTH_BYTES);

    request_header.insert(request_header.end(), file_name.begin(), file_name.end());
    return request_header;
}
