#include "input.h"
#include <vector>
#include <thread>
#include <mutex>
#ifdef __linux__
#include <pthread.h>
#elif _WIN32
#include <Windows.h>
#endif


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
	std::thread secondThread([&input, &mutex]() {
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
		std::cout << sum << std::endl;
	});
#ifdef __linux__
	sched_param threadParams;
	int policy;
	pthread_getshedparam(secondThread.native_hande(), &policy, &threadParams);
	threadParams.sched_priority = 10;
	pthread_setschedparam(secondThread.native_handle(), &policy, &threadParams);
#elif _WIN32
	SetThreadPriority(secondThread.native_handle(), THREAD_PRIORITY_LOWEST);
#endif
	firstThread.join();
	secondThread.join();
	return 0;
}