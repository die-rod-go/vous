#include <iostream>
#include <string>
#include "pulse.h"

int main(int argc, const char* argv[])
{
	Pulse pulse;
	//std::cout << argc << std::endl;

	if (argc > 2) {
		std::cout << "Usage: pulse [script]\n";
		return EXIT_FAILURE;
	}

	if (argc == 2) {
		pulse.runFile(std::string(argv[1]));
	}
	else {
		pulse.runPrompt();
	}

	std::cout << "quit\n";

	std::cin.get();
	return EXIT_SUCCESS;
}