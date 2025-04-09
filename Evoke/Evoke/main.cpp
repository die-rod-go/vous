#include <iostream>
#include <string>
#include "vous.h"

int main(int argc, const char* argv[])
{
	Vous vous;
	//std::cout << argc << std::endl;

	//	if too many arguments were provided
	if (argc > 2) {
		std::cout << "Usage: vous [script]\n";
		return EXIT_FAILURE;
	}

	//	filename specified
	if (argc == 2) {
		vous.runFile(std::string(argv[1]));
	}
	else {	//	go into interactive shell
		vous.runPrompt();
	}

	std::cout << "quit\n";

	std::cin.get();
	return EXIT_SUCCESS;
}