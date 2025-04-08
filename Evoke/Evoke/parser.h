#pragma once
#include <vector>
#include <memory>
#include <exception>
#include "token.h"
#include "expr.h"
#include "vous.h"
#include "stmt.h"

class ParseError : public std::exception {
public:

	Token token;
	std::string message;

	ParseError(Token token, std::string message) : token(token), message(message) {}
	const char* what() const noexcept override {
		return "An error occurred while parsing!";
	}
};

class Parser
{
public:
	std::vector<std::unique_ptr<Stmt>> parse();
	explicit Parser(std::vector<Token> tokens);
private:
	const std::vector<Token> tokens;
	int current = 0;
	std::unique_ptr<Stmt> declaration();
	std::unique_ptr<Stmt> varDeclaration();
	std::unique_ptr<Stmt> arrDeclaration();
	std::unique_ptr<Stmt> statement();
	std::unique_ptr<Stmt> printStatement();
	std::unique_ptr<Stmt> expressionStatement();
	std::unique_ptr<Stmt> functionStatement(const std::string& kind);
	std::unique_ptr<Stmt> blockStatement();
	std::unique_ptr<Stmt> ifStatement();
	std::unique_ptr<Stmt> whileStatement();
	std::unique_ptr<Stmt> forStatement();

	std::unique_ptr<Expr> expression();
	std::unique_ptr<Expr> assignment();
	std::unique_ptr<Expr> logical();
	std::unique_ptr<Expr> equality();
	std::unique_ptr<Expr> comparison();
	std::unique_ptr<Expr> term();
	std::unique_ptr<Expr> factor();
	std::unique_ptr<Expr> unary();
	std::unique_ptr<Expr> call();
	std::unique_ptr<Expr> finishCall(std::unique_ptr<Expr> callee);
	std::unique_ptr<Expr> primary();

	void synchronize();

	//	utility
	bool isAtEnd();
	Token advance();
	Token peek();
	Token previous();
	bool match(std::initializer_list<TokenType> types);
	bool check(TokenType type);
	Token consume(TokenType type, std::string message);

};