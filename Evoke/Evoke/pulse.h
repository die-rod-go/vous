#pragma once
#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "token.h"
#include "Scanner.h"
#include "interpreter.h"

class Pulse
{
public:
	Pulse();
	static bool hadError;
	static bool hadRuntimeError;
	static void error(int line, std::string message);
	static void error(Token token, std::string message);
	static void runtimeError(Token token, std::string message);

	void runFile(const std::string& path);
	void runPrompt();

private:
	void run(std::string source);
	static void report(int line, std::string location, std::string& message);
	static const Interpreter interpreter;
};
