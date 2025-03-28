#include "parser.h"
#include "pulse.h"

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
		Pulse::error(error.token, error.message);
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
		Pulse::error(error.token, error.message);
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

	consume(COLON, "Expect ':' after variable declaration.");
	Token subscribedEvent = consume(IDENTIFIER, "Expect event name after ':'.");
	consume(SEMICOLON, "Expect ';' after variable declaration");

	return std::make_unique<ByteStmt>(subscribedEvent, name, std::move(initializer));
}

std::unique_ptr<Stmt> Parser::arrDeclaration()
{
	Token name = consume(IDENTIFIER, "Expect array name.");
	consume(COLON, "Expect ':' after array declaration.");
	Token subscribedEvent = consume(IDENTIFIER, "Expect event name after ':'.");
	consume(SEMICOLON, "Expect ';' after variable declaration");
	return std::make_unique<ArrayStmt>(subscribedEvent, name);
}

std::unique_ptr<Stmt> Parser::statement()
{
	if (match({ PRINT })) return printStatement();
	if (match({ EMIT })) return evokeStatement();

	return expressionStatement();
}

std::unique_ptr<Stmt> Parser::printStatement()
{
	std::unique_ptr<Expr> value = expression();
	consume(COLON, "Expect ':' after print expression");
	Token subscribedEvent = consume(IDENTIFIER, "Expect event name after ':'.");
	consume(SEMICOLON, "Expect ';' after value.");
	return std::make_unique<PrintStmt>(subscribedEvent, std::move(value));
}

std::unique_ptr<Stmt> Parser::expressionStatement()
{
	std::unique_ptr<Expr> expr = expression();
	consume(COLON, "Expect ':' after expression.");
	Token subscribedEvent = consume(IDENTIFIER, "Expect event name after ':'.");
	consume(SEMICOLON, "Expect ';' after expression.");
	return std::make_unique<ExpressionStmt>(subscribedEvent, std::move(expr));
}

std::unique_ptr<Stmt> Parser::evokeStatement()
{
	Token eventName = consume(IDENTIFIER, "Expect event name after 'emit'.");
	std::unique_ptr<Expr> condition = nullptr;

	Token op;
	if (match({ QUESTION, QUESTION_QUESTION }))
	{
		op = previous();
		condition = expression();
	}

	Token subscribedEvent;
	if (match({ COLON }))
	{
		subscribedEvent = consume(IDENTIFIER, "Expect event name after ':'.");
	}

	consume(SEMICOLON, "Expect ';' after emit statement.");

	return std::make_unique<EmitStmt>(eventName, subscribedEvent, op, std::move(condition));
}

std::unique_ptr<Expr> Parser::expression()
{
	return assignment();
}

std::unique_ptr<Expr> Parser::assignment()
{
	std::unique_ptr<Expr> expr = equality();

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

		Pulse::error(equals, "Invalid assignment target.");
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
	if (match({ BANG }))
	{
		Token op = previous();
		auto right = unary();
		return std::make_unique<UnaryExpr>(op, std::move(right));
	}

	return primary();
}

std::unique_ptr<Expr> Parser::primary()
{
	if (match({ BYTE_LITERAL, STRING_LITERAL }))
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
		case EMIT:
		case CLEAR:
		case IDENTIFIER:
			return;
		}
		advance();
	}
}