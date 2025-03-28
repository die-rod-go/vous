#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "token.h"
#include "vous.h"
#include "value.h"


class Scanner
{
public:
	Scanner(std::string source);
	std::vector<Token> scanTokens();
	void printResult();

private:
	int start;
	int current;
	int line;

	//	where on the current line it is (for debugging purposes)
	int currentOnLine;

	const std::string source;
	std::vector<Token> tokens;

	bool isAtEnd();
	void scanToken();
	char advance();
	void addToken(TokenType type, Value literal);
	bool match(char expected);	//	checks if two characters are equal, advances if they are
	char peek();
	char peekNext();

	void handleDigit();
	void handleByteLiteral();
	void handleIdentifier();
	void handleString();

	Value binaryStringToValue(const std::string& binaryString);

	int getLength(int start, int current);

	static const std::unordered_map<std::string, TokenType> keywords;

	// debug
	void printCurrent();
};
