#include "input.h"
#include <vector>
#include <thread>

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
	std::thread firstThread([&input]() {
		input = UserInput();
		while (!IsCorrect(input)) {
			input = UserInput();
		}
		input = Processing(input);
	});
	firstThread.join();
	std::cout << input;
	return 0;
}