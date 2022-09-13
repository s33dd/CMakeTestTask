#include "input.h"
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
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
#define BUFFER_SIZE 1024


std::string Processing(std::string line) {
	const int size = static_cast<int>(line.length());
	std::vector<int> arr = { 0 };
	for (int i = 0; i < size; i++) {
		arr.push_back(line[i] - '0');
	}
	line.clear();
	std::sort(std::begin(arr), std::end(arr), std::greater<int>());
	for (int i = 0; i < size; i++) {
		if (arr[i] % 2 == 0) {
			line += "RnD";
		}
		else {
			line += std::to_string(arr[i]);
		}
	}
	return line;
}

void Test() {
	std::cout << "Test" << std::endl;
}

void SocketClose(int socket) {
#ifdef  __linux__
	close(socket);
#elif _WIN32
	closesocket(socket);
	WSACleanup();
#endif
}

int main(void) {
	int senderSock;
	sockaddr_in senderAddress;
	sockaddr_in handlerAddress;

#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	senderAddress.sin_family = AF_INET;
	senderSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (senderSock < 0) {
		std::cout << "ERROR: Socket was not created." << std::endl;
		SocketClose(senderSock);
		return 0;
	}
	senderAddress.sin_port = htons(DEFAULT_PORT);
	senderAddress.sin_addr.s_addr = htons(INADDR_ANY);
	if (bind(senderSock, reinterpret_cast<sockaddr*>(&senderAddress), sizeof(senderAddress)) < 0) {
		std::cout << "ERROR: Binding failed." << std::endl;
		SocketClose(senderSock);
		return 0;
	}
	if (listen(senderSock, 1) < 0) {
		std::cout << "ERROR: Listening failed." << std::endl;
		SocketClose(senderSock);
		return 0;
	}
	std::cout << "Waiting for handler" << std::endl;
#ifdef __linux__
	socklen_t handlerSize = sizeof(handlerAddress);
	senderSock = accept(senderSock, reinterpret_cast<sockaddr*>(&handlerAddress), &handlerSize);
	if (senderSock < 0) {
		std::cout << "ERROR: Accepting failed." << std::endl;
		SocketClose(senderSock);
		return 0;
	}
#elif _WIN32
	int handlerSize = sizeof(handlerAddress);
	senderSock = accept(senderSock, reinterpret_cast<sockaddr*>(&handlerAddress), &handlerSize);
	if (senderSock < 0) {
		std::cout << "ERROR: Accepting failed." << std::endl;
		SocketClose(senderSock);
		return 0;
	}
#endif
	std::cout << "Handler connected" << std::endl;
	while(true) {
		std::string input;
		std::mutex mutex;
		std::condition_variable condition;
		bool written = false;
		std::thread firstThread([&input, &mutex, &condition, &written]() {
			std::unique_lock lock(mutex);
			input = UserInput();
			while (!IsCorrect(input)) {
				input = UserInput();
			}
			input = Processing(input);
			written = true;
			condition.notify_one();
			});
		std::thread secondThread([&input, &mutex, &senderSock, &condition, &written]() {
			std::unique_lock lock(mutex);
			if(!written) {
				condition.wait(lock);
			}
			std::string line = input;
			input.clear();
			std::cout << "Received data: " << line << std::endl;
			const int size = static_cast<int>(line.length());
			int sum = 0;
			for (int i = 0; i < size; i++) {
				if (line[i] == 'R') {
					i += 2;
				}
				else {
					sum += line[i] - '0';
				}
			}
			std::string stringBuf = std::to_string(sum);
			char data[BUFFER_SIZE]{};
			for (int i = 0; i < stringBuf.length(); i++) {
				data[i] = stringBuf.c_str()[i];
			}
			send(senderSock, data, BUFFER_SIZE, 0);
			});
		firstThread.join();
		secondThread.join();
	}
}