#pragma once
#include <string>
#include <unordered_map>
#include "types.h"
#include "value.h"

enum TokenType {
	// Single-character tokens.
	LEFT_PAREN, RIGHT_PAREN, // ( )
	LEFT_BRACE, RIGHT_BRACE, // { }
	LEFT_BRACKET, RIGHT_BRACKET,
	COMMA, MINUS, PLUS, STAR, PERCENT,// , - + * % 
	SLASH, COLON, SEMICOLON, // / : ;

	// One or two character tokens.
	BANG, BANG_EQUAL, // ! !=
	EQUAL, EQUAL_EQUAL, // = ==
	GREATER, GREATER_EQUAL, GREATER_GREATER, // > >= >>
	LESS, LESS_EQUAL, LESS_LESS, // < <= <<
	ARROW, // <-
	AND, AND_AND,
	PIPE, PIPE_PIPE,
	QUESTION, QUESTION_QUESTION, // ? ??

	// Literals.
	IDENTIFIER, BYTE_LITERAL, STRING_LITERAL,

	// Keywords.
	EMIT, CLEAR, VAR, ARRAY, PRINT, INPUT,

	END_OF_FILE,

	NONE
};

class Token {
public:
	Token();
	Token(TokenType type, const std::string& lexeme, Value literal, int line);
	TokenType type;
	std::string lexeme;
	Value literal;
	int line;
	static const std::unordered_map<TokenType, std::string> enumStrings;
	std::string toString() const;

private:
};
