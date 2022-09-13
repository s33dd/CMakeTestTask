#include <iostream>
#include <string>
#ifdef __linux__
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#elif _WIN32
#include <Windows.h>
#include <winsock.h>
#endif
#define DEFAULT_PORT 1918
#define DEFAULT_IP "127.0.0.1"
#define BUFFER_SIZE 1024

void SocketClose(int socket) {
#ifdef  __linux__
	close(socket);
#elif _WIN32
	closesocket(socket);
	WSACleanup();
#endif
}

int main(void) {
	int handlerSock;
	sockaddr_in senderAddress;

#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	senderAddress.sin_family = AF_INET;
	senderAddress.sin_port = htons(DEFAULT_PORT);
	senderAddress.sin_addr.s_addr = inet_addr(DEFAULT_IP);

	handlerSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (handlerSock < 0) {
		std::cout << "ERROR: Socket was not created." << std::endl;
		SocketClose(handlerSock);
		return 0;
	}
	while(true) {
		std::cout << "Trying to connect" << std::endl;
#ifdef __linux__
		socklen_t senderSize = sizeof(senderAddress);
		if (connect(handlerSock, reinterpret_cast<sockaddr*>(&senderAddress), senderSize) != 0) {
			std::cout << "ERROR: Connection failed." << std::endl;
			sleep(1);
		}
		else {
			break;
		}
#elif _WIN32
		int senderSize = sizeof(senderAddress);
		if (connect(handlerSock, reinterpret_cast<sockaddr*>(&senderAddress), senderSize)) {
			std::cout << "ERROR: Connection failed." << std::endl;
			Sleep(1000);
		}
		else {
			break;
		}
#endif
	}
	std::cout << "Socket connected" << std::endl;
	while(true) {
		char data[BUFFER_SIZE];
		recv(handlerSock, data, BUFFER_SIZE, 0);
		std::string strData = data;
		int intData = std::stoi(strData);
		if (strData.length() > 2 && intData % 32 == 0) {
			std::cout << "Data was successfully received" << std::endl <<
				"Data: " << intData << std::endl;
		}
		else {
			std::cout << "ERROR: Wrong data" << std::endl;
		}
	}
}