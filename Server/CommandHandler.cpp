#include "CommandHandler.h"
#include "Protocol.h"
#include <fstream>

void CommandHandler::sendAll(SOCKET socket, char* data, int size){
	int total_sent = 0;
	while (total_sent < size) {
		int sent = send(socket, data + total_sent, size - total_sent, 0);
		if (sent <= 0){
			return;
		}
		total_sent += sent;
	}
}

