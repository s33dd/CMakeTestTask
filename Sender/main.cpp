#include "input.h"
#include <vector>
#include <thread>
#include <mutex>
#ifdef __linux__
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#elif _WIN32
#include <Windows.h>
#include <winsock.h>
#endif

#define DEFAULT_PORT 1918


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
		return 0;
	}
	senderAddress.sin_port = htons(DEFAULT_PORT);
	senderAddress.sin_addr.s_addr = htons(INADDR_ANY);
	if (bind(senderSock, reinterpret_cast<sockaddr*>(&senderAddress), sizeof(senderAddress)) < 0) {
		std::cout << "ERROR: Binding failed." << std::endl;
		return 0;
	}

	std::string input;
	std::mutex mutex;
	std::thread firstThread([&input, &mutex]() {
		mutex.lock();
		input = UserInput();
		while (!IsCorrect(input)) {
			input = UserInput();
		}
		input = Processing(input);
		mutex.unlock();
	});
	std::thread secondThread([&input, &mutex, &senderSock, &handlerAddress]() {
		mutex.lock();
		std::string line = input;
		input.clear();
		mutex.unlock();
		std::cout << "Received data: " << line << std::endl;
		const int size = static_cast<int>(line.length());
		int sum = 0;
		for(int i = 0; i < size; i++) {
			if(line[i] == 'R') {
				i += 2;
			}
			else {
				sum += line[i] - '0';
			}
		}
		listen(senderSock, 1);
		senderSock = accept(senderSock, reinterpret_cast<sockaddr*>(&handlerAddress), reinterpret_cast<int*>(sizeof(handlerAddress)));
		if (senderSock < 0 ) {
			std::cout << "ERROR: Connection failed." << std::endl;
		}
		std::cout << sum << std::endl;
	});
#ifdef __linux__
	sched_param threadParams;
	int policy;
	pthread_getschedparam(secondThread.native_handle(), &policy, &threadParams);
	threadParams.sched_priority = 10;
	pthread_setschedparam(secondThread.native_handle(), policy, &threadParams);
#elif _WIN32
	SetThreadPriority(secondThread.native_handle(), THREAD_PRIORITY_LOWEST);
#endif
	firstThread.join();
	secondThread.join();
#ifdef  __linux__
	close(senderSock);
#elif _WIN32
	closesocket(senderSock);
	WSACleanup();
#endif

	return 0;
}