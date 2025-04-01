#include "parser.h"
#include "vous.h"

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)), current(0) {}

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


std::unique_ptr<Stmt> Parser::declaration()
{
	try {
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

std::unique_ptr<Stmt> Parser::varDeclaration()
{
	Token name = consume(IDENTIFIER, "Expect variable name.");

	std::unique_ptr<Expr> initializer = nullptr;
	if (match({ EQUAL }))
	{
		initializer = expression();
	}

	consume(SEMICOLON, "Expect ';' after variable declaration");

	return std::make_unique<ByteStmt>(name, std::move(initializer));
}

std::unique_ptr<Stmt> Parser::arrDeclaration()
{
	Token name = consume(IDENTIFIER, "Expect array name.");
	consume(SEMICOLON, "Expect ';' after variable declaration");
	return std::make_unique<ArrayStmt>(name);
}

std::unique_ptr<Stmt> Parser::statement()
{
	if (match({ FOR })) return forStatement();
	if (match({ IF })) return ifStatement();
	if (match({ PRINT })) return printStatement();
	if (match({ WHILE })) return whileStatement();
	if (match({ LEFT_BRACE })) return blockStatement();

	return expressionStatement();
}

std::unique_ptr<Stmt> Parser::printStatement()
{
	std::unique_ptr<Expr> value = expression();
	consume(SEMICOLON, "Expect ';' after value.");
	return std::make_unique<PrintStmt>(std::move(value));
}

std::unique_ptr<Stmt> Parser::expressionStatement()
{
	std::unique_ptr<Expr> expr = expression();
	consume(SEMICOLON, "Expect ';' after expression.");
	return std::make_unique<ExpressionStmt>(std::move(expr));
}

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

std::unique_ptr<Stmt> Parser::whileStatement()
{
	consume(LEFT_PAREN, "Expect '(' after 'while'.");
	std::unique_ptr<Expr> condition = expression();
	consume(RIGHT_PAREN, "Expect ')' after while condition");
	std::unique_ptr<Stmt> body = statement();

	return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::forStatement()
{
	consume(LEFT_PAREN, "Expect '(' after 'for'.");
	std::unique_ptr<Stmt> initializer;

	//	parse initializer
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

	//	parse condition
	std::unique_ptr<Expr> condition = nullptr;
	if (!check(SEMICOLON))
	{
		condition = expression();
	}
	consume(SEMICOLON, "Expect ';' after loop condition.");

	//	parse clause
	std::unique_ptr<Expr> increment = nullptr;
	if (!check(RIGHT_PAREN))
	{
		increment = expression();
	}
	consume(RIGHT_PAREN, "Expect ')' after for clauses");

	std::unique_ptr<Stmt> body = statement();

	if (increment != nullptr)
	{
		//	construct list with body and increment as separate statements
		//	and set the loop body to be that new list
		std::vector<std::unique_ptr<Stmt>> statements;
		statements.push_back(std::move(body));
		statements.push_back(std::make_unique<ExpressionStmt>(std::move(increment)));
		body = std::make_unique<BlockStmt>(std::move(statements));
	}

	if (condition == nullptr) {
		condition = std::make_unique<LiteralExpr>(Token(TRUE, "true", Value(true), previous().line));
	}

	body = std::make_unique<WhileStmt>(std::move(condition), std::move(body));

	if (initializer != nullptr) {
		//	construct list with body and initializer as separate statements
		//	and set the loop body to be that new list
		std::vector<std::unique_ptr<Stmt>> statements;
		statements.push_back(std::move(initializer));
		statements.push_back(std::move(body));
		body = std::make_unique<BlockStmt>(std::move(statements));
	}

	return body;
}

std::unique_ptr<Expr> Parser::expression()
{
	return assignment();
}

std::unique_ptr<Expr> Parser::assignment()
{
	std::unique_ptr<Expr> expr = logical();

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

	if (match({ EQUAL }))
	{
		Token equals = previous();
		std::unique_ptr<Expr> value = assignment();

		if (ArrayAccessExpr* arrayAccess = dynamic_cast<ArrayAccessExpr*>(expr.get()))
		{
			Token name = arrayAccess->name;
			std::unique_ptr<Expr> index = std::move(arrayAccess->index);
			return std::make_unique<ArraySetExpr>(name, std::move(index), std::move(value));
		}

		//	instanceof (dirty idc)
		if (VariableExpr* variable = dynamic_cast<VariableExpr*>(expr.get()))
		{
			Token name = variable->name;
			return std::make_unique<AssignmentExpr>(name, std::move(value));
		}

		Vous::error(equals, "Invalid assignment target.");
	}
	return expr;
}

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

std::unique_ptr<Expr> Parser::factor()
{
	auto expr = unary();

	while (match({ STAR, SLASH , PERCENT }))
	{
		Token op = previous();
		auto right = unary();
		expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
	}

	return expr;
}

std::unique_ptr<Expr> Parser::unary()
{
	if (match({ MINUS, BANG }))
	{
		Token op = previous();
		auto right = unary();
		return std::make_unique<UnaryExpr>(op, std::move(right));
	}

	return primary();
}

std::unique_ptr<Expr> Parser::primary()
{
	if (match({ NUMBER_LITERAL, STRING_LITERAL, TRUE, FALSE }))
		return std::make_unique<LiteralExpr>(previous());

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

	if (match({ INPUT }))
		return std::make_unique<InputExpr>();

	if (match({ LEFT_PAREN }))
	{
		auto expr = expression();
		consume(RIGHT_PAREN, "Expected ')' after expression");
		return std::make_unique<GroupingExpr>(std::move(expr));
	}

	throw ParseError(peek(), "Expected expression");
	return nullptr;
}

bool Parser::isAtEnd()
{
	return peek().type == END_OF_FILE;
}

Token Parser::advance()
{
	if (!isAtEnd()) current++;
	return previous();
}

Token Parser::peek()
{
	return tokens[current];
}

Token Parser::previous()
{
	return tokens[current - 1];
}

bool Parser::check(TokenType type)
{
	if (isAtEnd()) return false;
	return peek().type == type;
}

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

//	consume expected token or report error
Token Parser::consume(TokenType type, std::string message)
{
	if (check(type)) return advance();
	throw ParseError(peek(), message);
	return peek();
}

//	synchronization for error recovery
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
			return;
		}
		advance();
	}
}