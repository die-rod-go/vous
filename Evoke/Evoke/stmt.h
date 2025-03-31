#pragma once
#include "expr.h"
#include <memory>

class StmtVisitor {
public:
	virtual void visit(const class PrintStmt& stmt) const = 0;
	virtual void visit(const class ExpressionStmt& stmt) const = 0;
	virtual void visit(const class ByteStmt& stmt) const = 0;
	virtual void visit(const class ArrayStmt& stmt) const = 0;
	virtual void visit(const class IfStmt& stmt) const = 0;
	virtual void visit(const class BlockStmt& stmt) const = 0;
};

class Stmt {
public:
	virtual ~Stmt() = default;
	virtual void accept(const StmtVisitor& visitor) const = 0;
};

class PrintStmt : public Stmt {
public:
	std::unique_ptr<Expr> expr;

	PrintStmt(std::unique_ptr<Expr> expr)
		: expr(std::move(expr)) {}

	void accept(const StmtVisitor& visitor) const override {
		visitor.visit(*this);
	}
};

class ExpressionStmt : public Stmt {
public:
	std::unique_ptr<Expr> expr;

	ExpressionStmt(std::unique_ptr<Expr> expr)
		: expr(std::move(expr)) {}

	void accept(const StmtVisitor& visitor) const override {
		visitor.visit(*this);
	}
};

class ByteStmt : public Stmt {
public:
	Token name;
	std::unique_ptr<Expr> initializer;

	ByteStmt(Token name, std::unique_ptr<Expr> initializer)
		: name(name), initializer(std::move(initializer)) {}

	void accept(const StmtVisitor& visitor) const override {
		visitor.visit(*this);
	}
};

class ArrayStmt : public Stmt {
public:
	Token name;

	ArrayStmt(Token name)
		: name(name) {}

	void accept(const StmtVisitor& visitor) const override {
		visitor.visit(*this);
	}
};

class IfStmt : public Stmt
{
public:
	std::unique_ptr<Expr> condition;
	std::unique_ptr<Stmt> thenBranch;
	std::unique_ptr<Stmt> elseBranch;

	IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
		: condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

	void accept(const StmtVisitor& visitor) const override {
		visitor.visit(*this);
	}

};

class BlockStmt : public Stmt
{
public:
	std::vector<std::unique_ptr<Stmt>> stmts;
	BlockStmt(std::vector<std::unique_ptr<Stmt>> stmts) : stmts(std::move(stmts)) {}

	void accept(const StmtVisitor& visitor) const override
	{
		visitor.visit(*this);
	}
};