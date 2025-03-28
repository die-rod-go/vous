#pragma once
#include "expr.h"
#include <memory>

class StmtVisitor {
public:
	virtual void visit(const class PrintStmt& stmt, bool evoked) const = 0;
	virtual void visit(const class ExpressionStmt& stmt, bool evoked) const = 0;
	virtual void visit(const class ByteStmt& stmt, bool evoked) const = 0;
	virtual void visit(const class ArrayStmt& stmt, bool evoked) const = 0;
	virtual void visit(const class EmitStmt& stmt, bool evoked) const = 0;
};

class Stmt {
public:
	virtual ~Stmt() = default;
	virtual void accept(const StmtVisitor& visitor, bool evoked) const = 0;
	virtual std::unique_ptr<Stmt> clone() const = 0;
};

class PrintStmt : public Stmt {
public:
	Token subscribedEvent;
	std::unique_ptr<Expr> expr;

	PrintStmt(Token subscribedEvent, std::unique_ptr<Expr> expr)
		: subscribedEvent(subscribedEvent), expr(std::move(expr)) {}

	void accept(const StmtVisitor& visitor, bool evoked) const override {
		visitor.visit(*this, evoked);
	}

	std::unique_ptr<Stmt> clone() const override {
		return std::make_unique<PrintStmt>(subscribedEvent, expr ? expr->clone() : nullptr);
	}
};

class ExpressionStmt : public Stmt {
public:
	Token subscribedEvent;
	std::unique_ptr<Expr> expr;

	ExpressionStmt(Token subscribedEvent, std::unique_ptr<Expr> expr)
		: subscribedEvent(subscribedEvent), expr(std::move(expr)) {}

	void accept(const StmtVisitor& visitor, bool evoked) const override {
		visitor.visit(*this, evoked);
	}

	std::unique_ptr<Stmt> clone() const override {
		return std::make_unique<ExpressionStmt>(subscribedEvent, expr ? expr->clone() : nullptr);
	}
};

class ByteStmt : public Stmt {
public:
	Token subscribedEvent;
	Token name;
	std::unique_ptr<Expr> initializer;

	ByteStmt(Token subscribedEvent, Token name, std::unique_ptr<Expr> initializer)
		: subscribedEvent(subscribedEvent), name(name), initializer(std::move(initializer)) {}

	void accept(const StmtVisitor& visitor, bool evoked) const override {
		visitor.visit(*this, evoked);
	}

	std::unique_ptr<Stmt> clone() const override {
		return std::make_unique<ByteStmt>(subscribedEvent, name, initializer ? initializer->clone() : nullptr);
	}
};

class ArrayStmt : public Stmt {
public:
	Token subscribedEvent;
	Token name;

	ArrayStmt(Token subscribedEvent, Token name)
		: subscribedEvent(subscribedEvent), name(name) {}

	void accept(const StmtVisitor& visitor, bool evoked) const override {
		visitor.visit(*this, evoked);
	}
	std::unique_ptr<Stmt> clone() const override {
		return std::make_unique<ArrayStmt>(subscribedEvent, name);
	}
};


class EmitStmt : public Stmt {
public:
	Token eventName;
	Token subscribedEvent;
	Token op;
	std::unique_ptr<Expr> condition;

	EmitStmt(Token eventName, Token subscribedEvent, Token op, std::unique_ptr<Expr> condition)
		: eventName(eventName), subscribedEvent(subscribedEvent), op(op), condition(std::move(condition)) {}

	EmitStmt(Token eventName, Token op, std::unique_ptr<Expr> condition)
		: eventName(eventName), subscribedEvent(Token()), op(op), condition(std::move(condition)) {}

	void accept(const StmtVisitor& visitor, bool evoked) const override {
		visitor.visit(*this, evoked);
	}

	std::unique_ptr<Stmt> clone() const override {
		return std::make_unique<EmitStmt>(eventName, op, condition ? condition->clone() : nullptr);
	}
};