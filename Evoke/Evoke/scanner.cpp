#include "Scanner.h"
#include <bitset>

const std::unordered_map<std::string, TokenType> Scanner::keywords = {
	{"emit", EMIT},
	{"clear", CLEAR},
	{"var", VAR},
	{"print", PRINT},
	{"[]", ARRAY},
	{"input", INPUT}
};

Scanner::Scanner(std::string source) : source(source), start(0), current(0), line(1), currentOnLine(0) {}

std::vector<Token> Scanner::scanTokens()
{
	while (!isAtEnd())
	{
		//	at the beginning of the next lexeme
		start = current;
		scanToken();
	}

	tokens.push_back(Token(END_OF_FILE, "", Value(), line));
	return tokens;
}

bool Scanner::isAtEnd()
{
	//printCurrent();
	return current >= source.length();
}

void Scanner::scanToken()
{
	//	current character we're looking at
	char c = advance();
	switch (c) {
		//	single character tokens
	case '(': addToken(LEFT_PAREN, Value()); break;
	case ')': addToken(RIGHT_PAREN, Value()); break;
	case '{': addToken(LEFT_BRACE, Value()); break;
	case '}': addToken(RIGHT_BRACE, Value()); break;
	case ']': addToken(RIGHT_BRACKET, Value()); break;
	case ',': addToken(COMMA, Value()); break;
	case '-': addToken(MINUS, Value()); break;
	case '+': addToken(PLUS, Value()); break;
	case '*': addToken(STAR, Value()); break;
	case '%': addToken(PERCENT, Value()); break;
	case ':': addToken(COLON, Value()); break;
	case ';': addToken(SEMICOLON, Value()); break;

		//	checks for tokens that could be single or part of 
		//	double character lexemes ex: ! vs != and > and >=
	case '[':
		addToken(match(']') ? ARRAY : LEFT_BRACKET, Value());
		break;
	case '!':
		addToken(match('=') ? BANG_EQUAL : BANG, Value());
		break;
	case '=':
		addToken(match('=') ? EQUAL_EQUAL : EQUAL, Value());
		break;
	case '<':
		if (match('='))
			addToken(LESS_EQUAL, Value());
		else if (match('<'))
			addToken(LESS_LESS, Value());
		else if (match('-'))
			addToken(ARROW, Value());
		else
			addToken(LESS, Value());
		break;
	case '>':
		if (match('='))
			addToken(GREATER_EQUAL, Value());
		else if (match('>>'))
			addToken(GREATER_GREATER, Value());
		else
			addToken(GREATER, Value());
		break;
	case '&':
		addToken(match('&') ? AND_AND : AND, Value());
		break;
	case '|':
		addToken(match('|') ? PIPE_PIPE : PIPE, Value());
		break;
	case '?':
		addToken(match('?') ? QUESTION_QUESTION : QUESTION, Value());
		break;

		//	checks for comments "//" and consumes the whole line
	case '/':
		if (match('/')) {
			//	A comment goes until the end of the line.
			while (peek() != '\n' && !isAtEnd())
				advance();
		}
		else {
			addToken(SLASH, Value());
		}
		break;

		//	ignore whitespace/meaningless characters
	case ' ':
	case '\r':
	case '\t':
		break;

	case '\n':
		line++;
		currentOnLine = 1;
		break;
		break;
	case '"':
		handleString(); break;
	case '0':
		if (match('b'))
		{
			handleByteLiteral();
		}
	default:
		if (isdigit(c))
			handleDigit();
		else if (isalpha(c) || c == '_')
			handleIdentifier();
		else
			Pulse::error(line, "Unexpected character: '" + std::string(1, c) + "'" + "at: " + std::to_string(currentOnLine));
	}
}

char Scanner::advance()
{
	currentOnLine++;
	return source[current++];
}

void Scanner::addToken(TokenType type, Value literal)
{
	int length = getLength(start, current);
	std::string text = source.substr(start, length);
	tokens.push_back(Token(type, text, literal, line));
}

//	checks if two characters are equal - conditional advance
bool Scanner::match(char expected)
{
	if (isAtEnd())
		return false;
	if (source[current] != expected)
		return false;

	currentOnLine++;
	current++;
	return true;
}

char Scanner::peek()
{
	if (isAtEnd())
		return '\0';
	return source[current];
}

char Scanner::peekNext()
{
	if (current + 1 >= source.length())
		return '\0';
	return source[current + 1];
}

void Scanner::handleByteLiteral()
{
	//	skip the 0b. only used to calculate the literal value, not the lexeme
	int bitStart = start + 2;

	//	could replace with "while (match('0') || match('1')) {}" but this felt less smelly
	while (peek() == '0' || peek() == '1')
		advance();

	int bitLength = getLength(bitStart, current);

	//	if the bit length is not exactly 8, report an error
	if (bitLength != 8) {
		Pulse::error(line, "Invalid byte literal. Expected 8 bits, got: " + std::to_string(bitLength));
		return;
	}

	//	extract only the 8-bit sequence (excluding "0b")
	std::string bitString = source.substr(bitStart, bitLength);

	//	convert and add the byte literal
	Value literal = binaryStringToValue(bitString);
	addToken(BYTE_LITERAL, literal);
}

void Scanner::handleDigit()
{
	while (isdigit(peek()))
		advance();
	int numLength = getLength(start, current);

	//	extract num as string
	std::string numString = source.substr(start, numLength);
	byte numAsByte = std::stoi(numString);
	addToken(BYTE_LITERAL, Value(numAsByte));
}

Value Scanner::binaryStringToValue(const std::string& binaryString)
{
	if (binaryString.length() != 8) {
		Pulse::error(line, "Invalid byte literal. Expected 8 bits, got: " + std::to_string(binaryString.length()));
		return 0;
	}
	return Value(static_cast<byte>(std::bitset<8>(binaryString).to_ulong()));
}

void Scanner::handleIdentifier()
{
	while (isalnum(peek()) || peek() == '_')
		advance();

	int length = getLength(start, current);
	TokenType type;
	std::string text = source.substr(start, length);

	//	if keyword is in map/exists in the language
	if (keywords.find(text) != keywords.end())
		type = keywords.at(text);
	else
		type = IDENTIFIER;

	addToken(type, Value());
}

// scanner.cpp
void Scanner::handleString() {
	while (peek() != '"' && !isAtEnd())
	{
		if (peek() == '\n')
		{
			line++;
			currentOnLine = 1;
		}
		advance();
	}

	if (isAtEnd())
	{
		Pulse::error(line, "Unterminated string");
		return;
	}

	//	the closing "
	advance();

	//	trim the surrounding quotes
	int length = getLength(start + 1, current - 1);
	std::string value = source.substr(start + 1, length);
	addToken(STRING_LITERAL, Value(value));
}

int Scanner::getLength(int start, int current)
{
	return current - start;
}

void Scanner::printCurrent()
{
	std::cout << "current: " << current << std::endl;
	std::cout << "line: " << line << std::endl;
	std::cout << "currentOnLine: " << currentOnLine << std::endl;
	std::cout << std::endl;
}

void Scanner::printResult()
{
	for (auto token : tokens)
	{
		std::cout << "==[" + Token::enumStrings.at(token.type) + "]==" << std::endl;
		std::cout << token.lexeme << std::endl;
	}
}