#include <iostream>
#include <sstream>
#include "Protocol.h"
#include "Client.h"

Command parseCommand(const std::string& str) {
    if (str == "GET") {
        return Command::Get;
    }
    else if (str == "LIST") {
        return Command::List;
    }
    else if (str == "PUT") {
        return Command::Put;
    }
    else if (str == "DELETE") {
        return Command::Delete;
    }
    else if (str == "INFO") {
        return Command::Info;
    }
    else {
        return Command::Unknown;
    }
}

int main() {
    Client client;
    client.run();

    std::string line;
    const int FILE_NAME_INDEX = 0;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);
        if (line.empty()) {
            continue;
        }

        std::istringstream iss(line);

        std::string client_name;
        iss >> client_name;

        std::string command;
        iss >> command;

        std::vector<std::string> args;
        std::string arg;
        while (iss >> arg) {
            args.push_back(arg);
        }

        Command cmd = parseCommand(command);
        switch (cmd) {
        case Command::Get:
            if (args.empty()) {
                std::cout << "Not enough arguments" << std::endl;
                break;
            }
            client.get(client_name, args[FILE_NAME_INDEX]);
            break;
        case Command::List:
            client.list(client_name);
            break;
        case Command::Put:
            if (args.empty()) {
                std::cout << "Not enough arguments" << std::endl;
                break;
            }
            client.put(client_name, args[FILE_NAME_INDEX]);
            break;
        case Command::Delete:
            if (args.empty()) {
                std::cout << "Not enough arguments" << std::endl;
                break;
            }
            client.deleteFile(client_name, args[FILE_NAME_INDEX]);
            break;
        case Command::Info:
            if (args.empty()) {
                std::cout << "Not enough arguments" << std::endl;
                break;
            }
            client.info(client_name, args[FILE_NAME_INDEX]);
            break;
        case Command::Unknown:
            std::cout << "Unknown command" << std::endl;
        }
    }
    return 0;
}