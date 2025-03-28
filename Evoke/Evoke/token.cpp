#include "Token.h"

const std::unordered_map<TokenType, std::string> Token::enumStrings = {
	{LEFT_PAREN, "LEFT_PAREN"}, {RIGHT_PAREN, "RIGHT_PAREN"},
	{LEFT_BRACE, "LEFT_BRACE"}, {RIGHT_BRACE, "RIGHT_BRACE"},
	{LEFT_BRACKET, "LEFT_BRACKET"}, {RIGHT_BRACKET, "RIGHT_BRACKET"},
	{COMMA, "COMMA"}, {MINUS, "MINUS"}, {PLUS, "PLUS"}, {STAR, "STAR"},
	{SLASH, "SLASH"}, {COLON, "COLON"}, {SEMICOLON, "SEMICOLON"},

	{BANG, "BANG"}, {BANG_EQUAL, "BANG_EQUAL"},
	{EQUAL, "EQUAL"}, {EQUAL_EQUAL, "EQUAL_EQUAL"},
	{GREATER, "GREATER"}, {GREATER_EQUAL, "GREATER_EQUAL"}, {GREATER_GREATER, "GREATER_GREATER"},
	{LESS, "LESS"}, {LESS_EQUAL, "LESS_EQUAL"}, {LESS_LESS, "LESS_LESS"},
	{ARROW, "ARROW"},
	{AND, "AND"}, {AND_AND, "AND_AND"},
	{PIPE, "PIPE"}, {PIPE_PIPE, "PIPE_PIPE"},
	{QUESTION, "QUESTION"}, {QUESTION_QUESTION, "QUESTION_QUESTION"},
	{IDENTIFIER, "IDENTIFIER"}, {NUMBER_LITERAL, "BYTE_LITERAL"}, {STRING_LITERAL, "STRING_LITERAL"},
	{VAR, "VAR"}, {ARRAY, "ARRAY"}, {PRINT, "PRINT"}, {INPUT, "INPUT"},
	{END_OF_FILE, "END_OF_FILE"},
	{NONE, "NONE"}
};

Token::Token()
	: type(TokenType::NONE), lexeme(""), literal(double(0)), line(-1) {}

Token::Token(TokenType type, const std::string& lexeme, Value literal, int line)
	: type(type), lexeme(lexeme), literal(literal), line(line) {}

std::string Token::toString() const {
	auto it = enumStrings.find(type);
	std::string typeStr = (it != enumStrings.end()) ? it->second : "UNKNOWN";
	return "type: " + typeStr + "\nlexeme: " + lexeme + "\nline: " + std::to_string(line);
}
