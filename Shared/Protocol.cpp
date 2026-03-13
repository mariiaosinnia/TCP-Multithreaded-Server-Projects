#include "Protocol.h"

bool recvAll(SOCKET socket, char* buffer, int size) {
	int total_received = 0;
	while (total_received < size) {
		int bytes_received = recv(socket, buffer + total_received, size - total_received, 0);
		if (bytes_received <= 0) {
			return false;
		}
		total_received += bytes_received;
	}
	return true;
}

bool sendAll(SOCKET socket, const char* buffer, int size) {
	int total_sent = 0;
	while (total_sent < size) {
		int bytes_sent = send(socket, buffer + total_sent, size - total_sent, 0);
		if (bytes_sent <= 0) {
			return false;
		}
		total_sent += bytes_sent;
	}
	return true;
}