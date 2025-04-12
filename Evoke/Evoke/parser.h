#pragma once
#include <vector>
#include <memory>
#include <exception>
#include "token.h"
#include "expr.h"
#include "vous.h"
#include "stmt.h"

/**
 * @class ParseError
 * @brief Represents an error that can occur while parsing.
 */
class ParseError : public std::exception {
public:

	Token token;
	std::string message;

	ParseError(Token token, std::string message) : token(token), message(message) {}

	const char* what() const noexcept override {
		return "An error occurred while parsing!";
	}
};

/**
 * @class Parser
 *
 * The Parser class goes through all the grammar rules of Vous and constructs an
 * AST based off the tokens fed to it during its construction.
 *
 * @brief Represents an object that takes in a list of Tokens and spits out an AST.
 */
class Parser
{
public:
	/** Creates new parser object with list of tokens */
	Parser(std::vector<Token> tokens);

	/** Spits out an AST based on the tokens the object was constructed with */
	std::vector<std::unique_ptr<Stmt>> parse();
private:
	// ==========[ Parsing Utilities ]===========
	//	These functions are helper functions for
	//	traversing the list of tokens

	/** Does what is says on the tin */
	bool isAtEnd();

	/** Returns the current token and updates to point to the next one */
	Token advance();

	/** Returns current token without advancing */
	Token peek();

	/** Returns previous token */
	Token previous();

	/** Checks if current token matches supplied type */
	bool check(TokenType type);

	/** Checks if current token is in the supplied list */
	bool match(std::initializer_list<TokenType> types);

	/** Consumes the current token and returns the next one */
	Token consume(TokenType type, std::string message);

	/** Advances until an anchor point is reached to continue parsing */
	void synchronize();

	// ==========[ Statements ]===========

	std::unique_ptr<Stmt> declaration();
	std::unique_ptr<Stmt> statement();
	std::unique_ptr<Stmt> expressionStatement();
	std::unique_ptr<Stmt> printStatement();
	std::unique_ptr<Stmt> varDeclaration();
	std::unique_ptr<Stmt> arrDeclaration();
	std::unique_ptr<Stmt> ifStatement();
	std::unique_ptr<Stmt> whileStatement();
	std::unique_ptr<Stmt> forStatement();
	std::unique_ptr<Stmt> functionStatement(const std::string& kind);
	std::unique_ptr<Stmt> blockStatement();

	// ==========[ Expressions ]===========

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


	// ==========[ Member Variables ]===========
	const std::vector<Token> tokens;
	int current = 0;
};