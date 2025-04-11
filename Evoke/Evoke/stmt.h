/*
This file defines all the different stmt nodes that could exist
as well as an abstract visitor class to visit the nodes
*/

#pragma once
#include "expr.h"
#include <memory>

class StmtVisitor {
public:
	virtual void visit(const class PrintStmt& stmt) const = 0;
	virtual void visit(const class ExpressionStmt& stmt) const = 0;
	virtual void visit(const class VariableStmt& stmt) const = 0;
	virtual void visit(const class ArrayStmt& stmt) const = 0;
	virtual void visit(const class BlockStmt& stmt) const = 0;
	virtual void visit(const class IfStmt& stmt) const = 0;
	virtual void visit(const class WhileStmt& stmt) const = 0;
	virtual void visit(const class FunctionStmt& stmt) const = 0;
};

class Stmt {
public:
	virtual ~Stmt() = default;
	virtual void accept(const StmtVisitor& visitor) const = 0;
	virtual std::unique_ptr<Stmt> clone() const = 0;
};


class PrintStmt : public Stmt {
public:
	std::unique_ptr<Expr> expr;

	PrintStmt(std::unique_ptr<Expr> expr)
		: expr(std::move(expr)) {}

	void accept(const StmtVisitor& visitor) const override {
		visitor.visit(*this);
	}

	std::unique_ptr<Stmt> clone() const override {
		return std::make_unique<PrintStmt>(expr->clone());
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

	std::unique_ptr<Stmt> clone() const override {
		return std::make_unique<ExpressionStmt>(expr->clone());
	}
};

class VariableStmt : public Stmt {
public:
	Token name;
	std::unique_ptr<Expr> initializer;

	VariableStmt(Token name, std::unique_ptr<Expr> initializer)
		: name(name), initializer(std::move(initializer)) {}

	void accept(const StmtVisitor& visitor) const override {
		visitor.visit(*this);
	}

	std::unique_ptr<Stmt> clone() const override {
		return std::make_unique<VariableStmt>(name, initializer ? initializer->clone() : nullptr);
	}
};

class ArrayStmt : public Stmt {
public:
	Token name;

	ArrayStmt(Token name) : name(name) {}

	void accept(const StmtVisitor& visitor) const override {
		visitor.visit(*this);
	}

	std::unique_ptr<Stmt> clone() const override {
		return std::make_unique<ArrayStmt>(name);
	}
};

class BlockStmt : public Stmt {
public:
	std::vector<std::unique_ptr<Stmt>> stmts;

	BlockStmt(std::vector<std::unique_ptr<Stmt>> stmts)
		: stmts(std::move(stmts)) {}

	void accept(const StmtVisitor& visitor) const override {
		visitor.visit(*this);
	}

	std::unique_ptr<Stmt> clone() const override {
		std::vector<std::unique_ptr<Stmt>> copied;
		for (const auto& stmt : stmts) {
			copied.push_back(stmt->clone());
		}
		return std::make_unique<BlockStmt>(std::move(copied));
	}
};

class IfStmt : public Stmt {
public:
	std::unique_ptr<Expr> condition;
	std::unique_ptr<Stmt> thenBranch;
	std::unique_ptr<Stmt> elseBranch;

	IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
		: condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

	void accept(const StmtVisitor& visitor) const override {
		visitor.visit(*this);
	}

	std::unique_ptr<Stmt> clone() const override {
		return std::make_unique<IfStmt>(
			condition->clone(),
			thenBranch->clone(),
			elseBranch ? elseBranch->clone() : nullptr
			);
	}
};

class WhileStmt : public Stmt {
public:
	std::unique_ptr<Expr> condition;
	std::unique_ptr<Stmt> body;

	WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
		: condition(std::move(condition)), body(std::move(body)) {}

	void accept(const StmtVisitor& visitor) const override {
		visitor.visit(*this);
	}

	std::unique_ptr<Stmt> clone() const override {
		return std::make_unique<WhileStmt>(
			condition->clone(),
			body->clone()
			);
	}
};

class FunctionStmt : public Stmt {
public:
	Token name;
	std::vector<Token> params;
	std::vector<std::unique_ptr<Stmt>> body;

	FunctionStmt(Token name, std::vector<Token> params, std::vector<std::unique_ptr<Stmt>> body)
		: name(name), params(params), body(std::move(body)) {}

	void accept(const StmtVisitor& visitor) const override {
		visitor.visit(*this);
	}

	std::unique_ptr<Stmt> clone() const override {
		std::vector<std::unique_ptr<Stmt>> copiedBody;
		for (const auto& stmt : body) {
			copiedBody.push_back(stmt->clone());
		}
		return std::make_unique<FunctionStmt>(name, params, std::move(copiedBody));
	}
};
