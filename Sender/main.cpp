#include "input.h"
#include <vector>

void Processing(std::string line) {
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
	std::cout << line;
}

int main(void) {
	std::string input = UserInput();
	while(!IsCorrect(input)) {
		input = UserInput();
	}
	Processing(input);
	return 0;
}