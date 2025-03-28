#include <iostream>
#include <string>
#include "vous.h"

int main(int argc, const char* argv[])
{
	Vous vous;
	//std::cout << argc << std::endl;

	if (argc > 2) {
		std::cout << "Usage: vous [script]\n";
		return EXIT_FAILURE;
	}

	if (argc == 2) {
		vous.runFile(std::string(argv[1]));
	}
	else {
		vous.runPrompt();
	}

	std::cout << "quit\n";

	std::cin.get();
	return EXIT_SUCCESS;
}