#include "RequestBuilder.h"
#include "Protocol.h"
#include <WinSock2.h>

std::vector<char> RequestBuilder::serializeCommand(Command command)
{
    std::vector<char> command_buf;
    command_buf.push_back(static_cast<uint8_t>(command));
    return command_buf;
}

std::vector<char> RequestBuilder::serializeFileName(const std::string& file_name)
{
    std::vector<char> file_name_buf;
    uint16_t file_name_length = static_cast<uint16_t>(file_name.length());
    uint16_t net_file_name_length = htons(file_name_length);
    file_name_buf.insert(file_name_buf.end(), reinterpret_cast<char*>(&net_file_name_length),
        reinterpret_cast<char*>(&net_file_name_length) + FILENAME_LENGTH_BYTES);

    file_name_buf.insert(file_name_buf.end(), file_name.begin(), file_name.end());
    return file_name_buf;
}

std::vector<char> RequestBuilder::serializeFileSize(uint32_t file_size)
{
    std::vector<char> file_size_buf;
    uint32_t net_file_size= htons(file_size);
    file_size_buf.insert(file_size_buf.end(), reinterpret_cast<char*>(&net_file_size),
        reinterpret_cast<char*>(&net_file_size) + FILE_SIZE_BYTES);
    return file_size_buf;
}

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
