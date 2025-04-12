#include "parser.h"
#include "vous.h"
#include <string>

/* ============================================ */
/*               Parser Construction            */
/* ============================================ */

/**
* @brief Constructs a Parser with the given tokens
* @param Vector of tokens to parse
*/
Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)), current(0) {}

/* ============================================ */
/*               Main Parse Function            */
/* ============================================ */

/**
* @brief Main parsing function that generates AST from tokens
*
* This is the entry point for parsing. It repeatedly calls declaration()
* until all tokens are consumed or an error occurs.
*
* @return Vector of parsed statements representing the program
* @throws ParseError if parsing fails
*/
std::vector<std::unique_ptr<Stmt>> Parser::parse()
{
	std::vector<std::unique_ptr<Stmt>> statements;
	try {
		while (!isAtEnd())
		{
			statements.push_back(declaration());
		}
	}
	catch (ParseError& error)
	{
		Vous::error(error.token, error.message);
	}
	return statements;
}

/* ============================================ */
/*            Token Navigation Helpers          */
/* ============================================ */

/**
* @brief Checks if we've reached the end of the token stream
* @return True if at EOF token, false otherwise
*/
bool Parser::isAtEnd()
{
	return peek().type == END_OF_FILE;
}

/**
* @brief Advances to the next token and returns the previous one
* @return The token before advancing
*/
Token Parser::advance()
{
	if (!isAtEnd()) current++;
	return previous();
}

/**
* @brief Returns the current token without consuming it
* @return Current token
*/
Token Parser::peek()
{
	return tokens[current];
}

/**
* @brief Returns the most recently consumed token
* @return Previous token
*/
Token Parser::previous()
{
	return tokens[current - 1];
}

/**
* @brief Checks if current token matches given type
* @param type Token type to check against
* @return True if current token matches type
*/
bool Parser::check(TokenType type)
{
	if (isAtEnd()) return false;
	return peek().type == type;
}

/**
* @brief Checks if current token matches any in list, consumes if matched
* @param types List of token types to check against
* @return True if any token matched and was consumed
*/
bool Parser::match(std::initializer_list<TokenType> types)
{
	for (auto type : types)
	{
		if (check(type))
		{
			advance();
			return true;
		}
	}
	return false;
}

/**
* @brief Consumes the current token if it matches expected type
* @param type Expected token type
* @param message Error message if type doesn't match
* @return The consumed token
* @throws ParseError if token doesn't match expected type
*/
Token Parser::consume(TokenType type, std::string message)
{
	if (check(type)) return advance();
	throw ParseError(peek(), message);
	return peek();
}

/**
* @brief Recovers from parse error by skipping to next statement boundary
*
* Advances until reaching a semicolon or a token that likely starts a new statement.
* Used for error recovery to continue parsing after an error.
*/
void Parser::synchronize()
{
	advance();

	while (!isAtEnd())
	{
		if (previous().type == SEMICOLON) return;
		switch (peek().type)
		{
		case VAR:
		case IDENTIFIER:
		case PRINT:
		case IF:
		case WHILE:
		case FOR:
		case FN:
			return;
		}
		advance();
	}
}

/* ============================================ */
/*            Declaration Parsing               */
/* ============================================ */

/**
* @brief Parses all types of declarations
*
* Handles function, variable, and array declarations. Falls back to statement()
* if no declaration keywords are found. Catches and reports parse errors.
*
* @return Parsed declaration statement
*/
std::unique_ptr<Stmt> Parser::declaration()
{
	try {
		if (match({ FN }))
		{
			return functionStatement("function");
		}
		if (match({ VAR }))
		{
			if (match({ ARRAY }))
				return arrDeclaration();
			else
				return varDeclaration();
		}
		return statement();
	}
	catch (ParseError error)
	{
		Vous::error(error.token, error.message);
		synchronize();
		return nullptr;
	}
}

/**
* @brief Parses a variable declaration
*
* Format: var identifier [= expression];
*
* @return Variable declaration statement
*/
std::unique_ptr<Stmt> Parser::varDeclaration()
{
	Token name = consume(IDENTIFIER, "Expect variable name.");
	std::unique_ptr<Expr> initializer = nullptr;
	if (match({ EQUAL }))
	{
		initializer = expression();
	}
	consume(SEMICOLON, "Expect ';' after variable declaration");
	return std::make_unique<VariableStmt>(name, std::move(initializer));
}

/**
* @brief Parses an array declaration
*
* Format: var[] (<- this is ARRAY) identifier;
*
* @return Array declaration statement
*/
std::unique_ptr<Stmt> Parser::arrDeclaration()
{
	Token name = consume(IDENTIFIER, "Expect array name.");
	consume(SEMICOLON, "Expect ';' after variable declaration");
	return std::make_unique<ArrayStmt>(name);
}

/**
* @brief Parses a function declaration
*
* Format: fn identifier(params) { body }
*
* @param kind String describing the function type (function/method for error messages)
* @return Function declaration statement
*/
std::unique_ptr<Stmt> Parser::functionStatement(const std::string& kind)
{
	int maxParameters = 255;
	Token name = consume(IDENTIFIER, "Expect " + kind + " name.");
	consume(LEFT_PAREN, "Expect '(' after " + kind + " name.");

	std::vector<Token> parameters;
	if (!check(RIGHT_PAREN))
	{
		do {
			if (parameters.size() >= maxParameters)
				Vous::error(peek(), "Can't have more than " + std::to_string(maxParameters) + " parameters");
			parameters.push_back(consume(IDENTIFIER, "Expect parameter name."));
		} while (match({ COMMA }));
	}
	consume(RIGHT_PAREN, "Expect ')' after parameters.");
	consume(LEFT_BRACE, "Expect '{' before " + kind + " body.");

	std::unique_ptr<Stmt> block = blockStatement();
	auto* blockStmt = dynamic_cast<BlockStmt*>(block.get());
	std::vector<std::unique_ptr<Stmt>> body = std::move(blockStmt->stmts);

	return std::make_unique<FunctionStmt>(name, parameters, std::move(body));
}

/* ============================================ */
/*              Statement Parsing               */
/* ============================================ */

/**
* @brief Parses any statement type
*
* Calls specific statement parsers based on current token.
* Falls back to expressionStatement() if no statement keywords are found.
*
* @return Parsed statement
*/
std::unique_ptr<Stmt> Parser::statement()
{
	if (match({ FOR })) return forStatement();
	if (match({ IF })) return ifStatement();
	if (match({ WHILE })) return whileStatement();
	if (match({ LEFT_BRACE })) return blockStatement();
	return expressionStatement();
}

/**
* @brief Parses an expression statement
*
* Format: expression;
*
* @return Expression statement
*/
std::unique_ptr<Stmt> Parser::expressionStatement()
{
	std::unique_ptr<Expr> expr = expression();
	consume(SEMICOLON, "Expect ';' after expression.");
	return std::make_unique<ExpressionStmt>(std::move(expr));
}

/**
* @brief Parses a block statement
*
* Format: { statements... }
*
* @return Block statement
*/
std::unique_ptr<Stmt> Parser::blockStatement()
{
	std::vector<std::unique_ptr<Stmt>> statements;
	while (!check(RIGHT_BRACE) && !isAtEnd())
	{
		statements.push_back(std::move(declaration()));
	}

	consume(RIGHT_BRACE, "Expect '}' after block.");
	return std::make_unique<BlockStmt>(std::move(statements));
}

/**
* @brief Parses an if statement
*
* Format: if (condition) statement [else statement]
*
* @return If statement
*/
std::unique_ptr<Stmt> Parser::ifStatement()
{
	consume(LEFT_PAREN, "Expect '(' after 'if'");
	std::unique_ptr<Expr> condition = expression();
	consume(RIGHT_PAREN, "Expect ')' after if condition");

	std::unique_ptr<Stmt> thenBranch = statement();
	std::unique_ptr<Stmt> elseBranch = nullptr;
	if (match({ ELSE }))
		elseBranch = statement();

	return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

/**
* @brief Parses a while statement
*
* Format: while (condition) statement
*
* @return While statement
*/
std::unique_ptr<Stmt> Parser::whileStatement()
{
	consume(LEFT_PAREN, "Expect '(' after 'while'.");
	std::unique_ptr<Expr> condition = expression();
	consume(RIGHT_PAREN, "Expect ')' after while condition");
	std::unique_ptr<Stmt> body = statement();

	return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

/**
* @brief Parses a for statement
*
* Format: for ([initializer]; [condition]; [increment]) statement
*
* The for loop is desugared into a while loop with proper scoping.
*
* @return For statement (converted to while statement)
*/
std::unique_ptr<Stmt> Parser::forStatement()
{
	consume(LEFT_PAREN, "Expect '(' after 'for'.");
	std::unique_ptr<Stmt> initializer;

	// Parse initializer (optional)
	if (match({ SEMICOLON }))
	{
		initializer = nullptr;
	}
	else if (match({ VAR }))
	{
		initializer = varDeclaration();
	}
	else
	{
		initializer = expressionStatement();
	}

	// Parse condition (optional)
	std::unique_ptr<Expr> condition = nullptr;
	if (!check(SEMICOLON))
	{
		condition = expression();
	}
	consume(SEMICOLON, "Expect ';' after loop condition.");

	// Parse increment (optional)
	std::unique_ptr<Expr> increment = nullptr;
	if (!check(RIGHT_PAREN))
	{
		increment = expression();
	}
	consume(RIGHT_PAREN, "Expect ')' after for clauses");

	std::unique_ptr<Stmt> body = statement();

	// Desugar for loop into while loop
	if (increment != nullptr)
	{
		// Add increment as last statement in body
		std::vector<std::unique_ptr<Stmt>> statements;
		statements.push_back(std::move(body));
		statements.push_back(std::make_unique<ExpressionStmt>(std::move(increment)));
		body = std::make_unique<BlockStmt>(std::move(statements));
	}

	// Default condition to true if omitted
	if (condition == nullptr) {
		condition = std::make_unique<LiteralExpr>(Token(TRUE, "true", Value(true), previous().line));
	}

	body = std::make_unique<WhileStmt>(std::move(condition), std::move(body));

	// Wrap with initializer if present
	if (initializer != nullptr) {
		std::vector<std::unique_ptr<Stmt>> statements;
		statements.push_back(std::move(initializer));
		statements.push_back(std::move(body));
		body = std::make_unique<BlockStmt>(std::move(statements));
	}

	return body;
}

/* ============================================ */
/*              Expression Parsing              */
/* ============================================ */

/**
* @brief Entry point for expression parsing
* @return Parsed expression
*/
std::unique_ptr<Expr> Parser::expression()
{
	return assignment();
}

/**
* @brief Parses assignment expressions
*
* Handles both variable assignment and array operations:
* - identifier = expression
* - identifier[index] = expression
* - identifier <- expression (array push)
*
* @return Parsed expression
*/
std::unique_ptr<Expr> Parser::assignment()
{
	std::unique_ptr<Expr> expr = logical();

	// Handle array push operation
	if (match({ ARROW }))
	{
		Token arrow = previous();
		std::unique_ptr<Expr> value = assignment();
		if (VariableExpr* variable = dynamic_cast<VariableExpr*>(expr.get()))
		{
			Token name = variable->name;
			return std::make_unique<ArrayPushExpr>(name, std::move(value));
		}
	}

	// Handle regular assignment
	if (match({ EQUAL }))
	{
		Token equals = previous();
		std::unique_ptr<Expr> value = assignment();

		// Array element assignment
		if (ArrayAccessExpr* arrayAccess = dynamic_cast<ArrayAccessExpr*>(expr.get()))
		{
			Token name = arrayAccess->name;
			std::unique_ptr<Expr> index = std::move(arrayAccess->index);
			return std::make_unique<ArraySetExpr>(name, std::move(index), std::move(value));
		}

		// Regular variable assignment
		if (VariableExpr* variable = dynamic_cast<VariableExpr*>(expr.get()))
		{
			Token name = variable->name;
			return std::make_unique<AssignmentExpr>(name, std::move(value));
		}

		Vous::error(equals, "Invalid assignment target.");
	}
	return expr;
}

/* ============================================ */
/*           Expression Sub-Parsers             */
/* ============================================ */

// The following functions implement the operator precedence hierarchy
// Each level delegates to the next higher precedence level and then
// handles its own operators.

/**
* @brief Parses logical OR and AND expressions
* @return Parsed expression
*/
std::unique_ptr<Expr> Parser::logical()
{
	auto expr = equality();

	while (match({ AND_AND, PIPE_PIPE }))
	{
		Token op = previous();
		auto right = equality();
		expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
	}

	return expr;
}

/**
* @brief Parses equality comparisons (==, !=)
* @return Parsed expression
*/
std::unique_ptr<Expr> Parser::equality()
{
	auto expr = comparison();

	while (match({ BANG_EQUAL, EQUAL_EQUAL }))
	{
		Token op = previous();
		auto right = comparison();
		expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
	}

	return expr;
}

/**
* @brief Parses comparison operators (>, >=, <, <=)
* @return Parsed expression
*/
std::unique_ptr<Expr> Parser::comparison()
{
	auto expr = term();

	while (match({ GREATER, GREATER_EQUAL, LESS, LESS_EQUAL, EQUAL_EQUAL }))
	{
		Token op = previous();
		auto right = term();
		expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
	}

	return expr;
}

/**
* @brief Parses addition and subtraction
* @return Parsed expression
*/
std::unique_ptr<Expr> Parser::term()
{
	auto expr = factor();

	while (match({ PLUS, MINUS }))
	{
		Token op = previous();
		auto right = factor();
		expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
	}

	return expr;
}

/**
* @brief Parses multiplication, division, and modulus
* @return Parsed expression
*/
std::unique_ptr<Expr> Parser::factor()
{
	auto expr = unary();

	while (match({ STAR, SLASH, PERCENT }))
	{
		Token op = previous();
		auto right = unary();
		expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
	}

	return expr;
}

/**
* @brief Parses unary operators (-, !)
* @return Parsed expression
*/
std::unique_ptr<Expr> Parser::unary()
{
	if (match({ MINUS, BANG }))
	{
		Token op = previous();
		auto right = unary();
		return std::make_unique<UnaryExpr>(op, std::move(right));
	}

	return call();
}

/**
* @brief Parses function calls and array accesses
* @return Parsed expression
*/
std::unique_ptr<Expr> Parser::call()
{
	std::unique_ptr<Expr> expr = primary();

	while (true)
	{
		if (match({ LEFT_PAREN }))
		{
			expr = finishCall(std::move(expr));
		}
		else
		{
			break;
		}
	}
	return expr;
}

/**
* @brief Completes parsing a function call after the opening paren
* @param callee The expression being called
* @return Call expression
*/
std::unique_ptr<Expr> Parser::finishCall(std::unique_ptr<Expr> callee)
{
	int maxArguments = 255;

	std::vector<std::unique_ptr<Expr>> arguments;
	if (!check(RIGHT_PAREN))
	{
		do {
			if (arguments.size() >= maxArguments)
				Vous::error(peek(), "Can't have more than 255 arguments");
			arguments.push_back(std::move(expression()));
		} while (match({ COMMA }));
	}

	Token paren = consume(RIGHT_PAREN, "Expect ')' after arguments.");

	return std::make_unique<CallExpr>(std::move(callee), paren, std::move(arguments));
}

/**
* @brief Parses primary expressions (literals, variables, grouping)
* @return Parsed expression
* @throws ParseError if no valid primary expression found
*/
std::unique_ptr<Expr> Parser::primary()
{
	// Literal values
	if (match({ NUMBER_LITERAL, STRING_LITERAL, TRUE, FALSE }))
		return std::make_unique<LiteralExpr>(previous());

	// Variables and array accesses
	if (match({ IDENTIFIER }))
	{
		Token name = previous();
		if (match({ LEFT_BRACKET }))
		{
			std::unique_ptr<Expr> index = expression();
			consume(RIGHT_BRACKET, "Expect ']' after array index.");
			return std::make_unique<ArrayAccessExpr>(name, std::move(index));
		}
		return std::make_unique<VariableExpr>(previous());
	}

	// Grouped expressions
	if (match({ LEFT_PAREN }))
	{
		auto expr = expression();
		consume(RIGHT_PAREN, "Expected ')' after expression");
		return std::make_unique<GroupingExpr>(std::move(expr));
	}

	throw ParseError(peek(), "Expected expression");
	return nullptr;
}