#include "Scanner.h"
#include <bitset>

const std::unordered_map<std::string, TokenType> Scanner::keywords = {
	{"var", VAR},
	//{"print", PRINT},
	{"[]", ARRAY},
	//{"input", INPUT},
	{"true", TRUE},
	{"false", FALSE},
	{"if", IF},
	{"else", ELSE},
	{"while", WHILE},
	{"for",  FOR},
	{"fn", FN}
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
		else if (match('>'))
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
	default:
		if (isdigit(c))
			handleDigit();
		else if (isalpha(c) || c == '_')
			handleIdentifier();
		else
			Vous::error(line, "Unexpected character: '" + std::string(1, c) + "'" + "at: " + std::to_string(currentOnLine));
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

void Scanner::handleDigit()
{
	while (isdigit(peek()))
		advance();

	if (peek() == '.' && isdigit(peekNext()))
	{
		advance();
		while (isdigit(peek()))
			advance();
	}

	int length = getLength(start, current);
	double value = std::stod(source.substr(start, length));
	addToken(TokenType::NUMBER_LITERAL, Value(value));
}

void Scanner::handleIdentifier()
{
	while (isalnum(peek()) || peek() == '_')
		advance();

	int length = getLength(start, current);
	Value val = Value();
	TokenType type;
	std::string text = source.substr(start, length);

	//	if keyword is in map/exists in the language
	if (keywords.find(text) != keywords.end())
	{
		type = keywords.at(text);
		if (type == TRUE)
			val = Value(true);
		else if (type == FALSE)
			val = Value(false);
	}
	else
		type = IDENTIFIER;

	addToken(type, val);
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
		Vous::error(line, "Unterminated string");
		return;
	}

	//	the closing "
	advance();

	//	trim the surrounding quotes
	int length = getLength(start + 1, current - 1);
	std::string value = source.substr(start + 1, length);
	addToken(STRING_LITERAL, Value(value));
}

void Scanner::handleBool()
{
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