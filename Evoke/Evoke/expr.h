#pragma once
#include "token.h"
#include <memory>

class ExprVisitor {
public:
	virtual void visit(const class UnaryExpr& expr) const = 0;
	virtual void visit(const class BinaryExpr& expr) const = 0;
	virtual void visit(const class GroupingExpr& expr) const = 0;
	virtual void visit(const class LiteralExpr& expr) const = 0;
	virtual void visit(const class VariableExpr& expr) const = 0;
	virtual void visit(const class AssignmentExpr& expr) const = 0;
	virtual void visit(const class ArrayPushExpr& expr) const = 0;
	virtual void visit(const class ArrayAccessExpr& expr) const = 0;
	virtual void visit(const class ArraySetExpr& expr) const = 0;
	virtual void visit(const class InputExpr& expr) const = 0;

};

class Expr
{
public:
	virtual ~Expr() = default;
	virtual void accept(const ExprVisitor& visitor) const = 0;
	virtual std::unique_ptr<Expr> clone() const = 0;
};

class UnaryExpr : public Expr
{
public:
	Token op;
	std::unique_ptr<Expr> operand;

	UnaryExpr(Token op, std::unique_ptr<Expr> operand)
		: op(op), operand(std::move(operand)) {}

	void accept(const ExprVisitor& visitor) const override
	{
		visitor.visit(*this);
	}

	std::unique_ptr<Expr> clone() const override {
		return std::make_unique<UnaryExpr>(op, operand ? operand->clone() : nullptr);
	}
};

class BinaryExpr : public Expr
{
public:
	std::unique_ptr<Expr> left;
	Token op;
	std::unique_ptr<Expr> right;

	BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
		: left(std::move(left)), op(op), right(std::move(right)) {}

	void accept(const ExprVisitor& visitor) const override
	{
		visitor.visit(*this);
	}

	std::unique_ptr<Expr> clone() const override {
		return std::make_unique<BinaryExpr>(left ? left->clone() : nullptr, op, right ? right->clone() : nullptr);
	}
};

class GroupingExpr : public Expr
{
public:
	std::unique_ptr<Expr> expr;

	GroupingExpr(std::unique_ptr<Expr> expr) : expr(std::move(expr)) {}

	void accept(const ExprVisitor& visitor) const override
	{
		visitor.visit(*this);
	}

	std::unique_ptr<Expr> clone() const override {
		return std::make_unique<GroupingExpr>(expr ? expr->clone() : nullptr);
	}
};

class LiteralExpr : public Expr
{
public:
	Token literal;
	explicit LiteralExpr(Token literal) : literal(literal) {}
	void accept(const ExprVisitor& visitor) const override
	{
		visitor.visit(*this);
	}

	std::unique_ptr<Expr> clone() const override {
		return std::make_unique<LiteralExpr>(literal);
	}
};

class VariableExpr : public Expr
{
public:
	Token name;
	explicit VariableExpr(Token name) : name(name) {}
	void accept(const ExprVisitor& visitor) const override
	{
		visitor.visit(*this);
	}

	std::unique_ptr<Expr> clone() const override {
		return std::make_unique<VariableExpr>(name);
	}
};

class AssignmentExpr : public Expr
{
public:
	Token name;
	std::unique_ptr<Expr> value;

	AssignmentExpr(Token name, std::unique_ptr<Expr> value)
		: name(name), value(std::move(value)) {}

	void accept(const ExprVisitor& visitor) const override
	{
		visitor.visit(*this);
	}

	std::unique_ptr<Expr> clone() const override {
		return std::make_unique<AssignmentExpr>(name, value ? value->clone() : nullptr);
	}
};

class ArrayPushExpr : public Expr {
public:
	Token name;
	std::unique_ptr<Expr> value;
	ArrayPushExpr(Token name, std::unique_ptr<Expr> value)
		: name(name), value(std::move(value)) {}

	void accept(const ExprVisitor& visitor) const override
	{
		visitor.visit(*this);
	}

	std::unique_ptr<Expr> clone() const override {
		return std::make_unique<ArrayPushExpr>(name, value ? value->clone() : nullptr);
	}
};

class ArrayAccessExpr : public Expr {
public:
	Token name;
	std::unique_ptr<Expr> index;
	ArrayAccessExpr(Token name, std::unique_ptr<Expr> index)
		: name(name), index(std::move(index)) {}

	void accept(const ExprVisitor& visitor) const override
	{
		visitor.visit(*this);
	}

	std::unique_ptr<Expr> clone() const override {
		return std::make_unique<ArrayAccessExpr>(name, index ? index->clone() : nullptr);
	}
};

class ArraySetExpr : public Expr {
public:
	Token name;
	std::unique_ptr<Expr> index;
	std::unique_ptr<Expr> value;

	ArraySetExpr(Token name, std::unique_ptr<Expr> index, std::unique_ptr<Expr> value)
		: name(name), index(std::move(index)), value(std::move(value)) {}

	void accept(const ExprVisitor& visitor) const override
	{
		visitor.visit(*this);
	}

	std::unique_ptr<Expr> clone() const override {
		return std::make_unique<ArraySetExpr>(name, index ? index->clone() : nullptr, value ? value->clone() : nullptr);
	}
};

class InputExpr : public Expr {
public:
	InputExpr() {};

	void accept(const ExprVisitor& visitor) const override
	{
		visitor.visit(*this);
	}

	std::unique_ptr<Expr> clone() const override {
		return std::make_unique<InputExpr>();
	}
};