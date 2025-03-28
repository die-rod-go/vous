#include "pulse.h"
#include "parser.h"
#include "ASTPrinter.h"

bool Pulse::hadError = false;
bool Pulse::hadRuntimeError = false;
const Interpreter Pulse::interpreter = Interpreter();

Pulse::Pulse()
{

}

void Pulse::error(int line, std::string message)
{
	report(line, "", message);
}

void Pulse::error(Token token, std::string message)
{
	if (token.type == END_OF_FILE)
		report(token.line, " at end", message);
	else
		report(token.line, " at '" + token.lexeme + "'", message);
}

void Pulse::runtimeError(Token token, std::string message)
{
	hadRuntimeError = true;
	std::cout << "[line " << token.line << "] Error: " << message << std::endl;
}

void Pulse::runFile(const std::string& path)
{
	//	open file
	std::ifstream myFile(path);
	if (!myFile.is_open())
		std::cout << "Error opening file" << std::endl;

	//	convert to string stream
	std::stringstream buffer;
	//	read string stream into string
	buffer << myFile.rdbuf();

	myFile.close();

	run(buffer.str());	//	run the file
	if (hadError)
		exit(EXIT_FAILURE);
	if (hadRuntimeError)
		exit(EXIT_FAILURE);
}

void Pulse::runPrompt()
{
	std::string line;

	while (true)
	{
		//	get input from user
		std::cout << ">";
		std::getline(std::cin, line);

		if (line == "")
			break;	//	quit if no input
		//	run input
		run(line);
		//	reset hadError so that syntax errors don't kill the session
		hadError = false;
		hadRuntimeError = false;
	}
}

void Pulse::run(std::string source)
{
	Scanner scanner(source);
	std::vector<Token> tokens = scanner.scanTokens();
	//scanner.printResult();

	Parser parser(tokens);
	std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

	if (hadError) return;

	interpreter.interpret(statements);

}

void Pulse::report(int line, std::string location, std::string& message)
{
	std::cout << "[line " << line << "] Error" << location << ": " << message << std::endl;
	hadError = true;
}


