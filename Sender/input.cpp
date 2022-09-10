#include "input.h"

std::string UserInput() {
	std::cout << "Please, input string. Pay attention that string must contains only numbers " <<
		"and contains not more than 64 symbols." << std::endl;
	std::cout << "Your input: ";
	std::string input;
	std::getline(std::cin, input);
	std::cout << std::endl;
	return input;
}

bool IsCorrect(std::string line) {
	if (line.length() > 64) {
		return false;
	}
	const std::regex exp("\\d+");
	return std::regex_match(line, exp);
}
