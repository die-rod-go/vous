/*
This file defines all the different expr nodes that could exist
as well as an abstract visitor class to visit the nodes
*/

#pragma once
#include "token.h"
#include <memory>

/**
 * @class ExprVisitor
 * @brief Abstract base class for visiting expression nodes.
 *
 * This class provides the interface for visiting different types of expression nodes.
 */
class ExprVisitor {
public:
	virtual void visit(const class AssignmentExpr& expr) const = 0;
	virtual void visit(const class BinaryExpr& expr) const = 0;
	virtual void visit(const class CallExpr& expr) const = 0;
	virtual void visit(const class GroupingExpr& expr) const = 0;
	virtual void visit(const class LiteralExpr& expr) const = 0;
	virtual void visit(const class UnaryExpr& expr)		const = 0;
	virtual void visit(const class VariableExpr& expr) const = 0;
	
	//	will be reworked
	virtual void visit(const class ArrayPushExpr& expr) const = 0;
	virtual void visit(const class ArrayAccessExpr& expr) const = 0;
	virtual void visit(const class ArraySetExpr& expr) const = 0;
	
	//	should not be used
	virtual void visit(const class InputExpr& expr) const = 0;
};

/**
 * @class Expr
 * @brief Abstract base class for all expression types.
 */
class Expr {
public:
	virtual ~Expr() = default;
	virtual void accept(const ExprVisitor& visitor) const = 0;
	virtual std::unique_ptr<Expr> clone() const = 0;
};

/**
 * @class UnaryExpr
 * @brief Represents a unary expression with a single operand and an operator.
 */
class UnaryExpr : public Expr {
public:
	/** The operator token for this unary expression. */
	Token op;

	/** The operand expression. */
	std::unique_ptr<Expr> operand;

	UnaryExpr(Token op, std::unique_ptr<Expr> operand)
		: op(op), operand(std::move(operand)) {}

	void accept(const ExprVisitor& visitor) const override {
		visitor.visit(*this);
	}

	std::unique_ptr<Expr> clone() const override {
		return std::make_unique<UnaryExpr>(op, operand->clone());
	}
};

/**
 * @class BinaryExpr
 * @brief Represents a binary expression with a left operand, an operator, and a right operand.
 */
class BinaryExpr : public Expr {
public:
	/** The left operand expression. */
	std::unique_ptr<Expr> left;

	/** The operator token for this binary expression. */
	Token op;

	/** The right operand expression. */
	std::unique_ptr<Expr> right;

	BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
		: left(std::move(left)), op(op), right(std::move(right)) {}

	void accept(const ExprVisitor& visitor) const override {
		visitor.visit(*this);
	}

	std::unique_ptr<Expr> clone() const override {
		return std::make_unique<BinaryExpr>(left->clone(), op, right->clone());
	}
};

/**
 * @class GroupingExpr
 * @brief Represents a grouping expression ( parentheses ) around an expression.
 */
class GroupingExpr : public Expr {
public:
	/** The expression inside the grouping. */
	std::unique_ptr<Expr> expr;

	GroupingExpr(std::unique_ptr<Expr> expr)
		: expr(std::move(expr)) {}

	void accept(const ExprVisitor& visitor) const override {
		visitor.visit(*this);
	}

	std::unique_ptr<Expr> clone() const override {
		return std::make_unique<GroupingExpr>(expr->clone());
	}
};

/**
 * @class LiteralExpr
 * @brief Represents a literal value (e.g., number, string, etc.).
 */
class LiteralExpr : public Expr {
public:
	/** The literal token. */
	Token literal;

	LiteralExpr(Token literal) : literal(literal) {}

	void accept(const ExprVisitor& visitor) const override {
		visitor.visit(*this);
	}

	std::unique_ptr<Expr> clone() const override {
		return std::make_unique<LiteralExpr>(literal);
	}
};

/**
 * @class VariableExpr
 * @brief Represents a variable access expression.
 * 
 * var x = 10;
 * print("x") <- x here is the variable expr
 */
class VariableExpr : public Expr {
public:
	/** The variable's name. */
	Token name;

	VariableExpr(Token name) : name(name) {}

	void accept(const ExprVisitor& visitor) const override {
		visitor.visit(*this);
	}

	std::unique_ptr<Expr> clone() const override {
		return std::make_unique<VariableExpr>(name);
	}
};

/**
 * @class AssignmentExpr
 * @brief Represents an assignment expression.
 * 
 * x = 10; <- this whole line is the assignment expr
 * Stores a token with the variable name and and a
 * pointer to an expression which evaluates to the value
 */
class AssignmentExpr : public Expr {
public:
	Token name;
	std::unique_ptr<Expr> value;

	AssignmentExpr(Token name, std::unique_ptr<Expr> value)
		: name(name), value(std::move(value)) {}

	void accept(const ExprVisitor& visitor) const override {
		visitor.visit(*this);
	}

	std::unique_ptr<Expr> clone() const override {
		return std::make_unique<AssignmentExpr>(name, value->clone());
	}
};

//	will be reworked
class ArrayPushExpr : public Expr {
public:
	Token name;
	std::unique_ptr<Expr> value;

	ArrayPushExpr(Token name, std::unique_ptr<Expr> value)
		: name(name), value(std::move(value)) {}

	void accept(const ExprVisitor& visitor) const override {
		visitor.visit(*this);
	}

	std::unique_ptr<Expr> clone() const override {
		return std::make_unique<ArrayPushExpr>(name, value->clone());
	}
};

//	will be reworked
class ArrayAccessExpr : public Expr {
public:
	Token name;
	std::unique_ptr<Expr> index;

	ArrayAccessExpr(Token name, std::unique_ptr<Expr> index)
		: name(name), index(std::move(index)) {}

	void accept(const ExprVisitor& visitor) const override {
		visitor.visit(*this);
	}

	std::unique_ptr<Expr> clone() const override {
		return std::make_unique<ArrayAccessExpr>(name, index->clone());
	}
};

//	will be reworked
class ArraySetExpr : public Expr {
public:
	Token name;
	std::unique_ptr<Expr> index;
	std::unique_ptr<Expr> value;

	ArraySetExpr(Token name, std::unique_ptr<Expr> index, std::unique_ptr<Expr> value)
		: name(name), index(std::move(index)), value(std::move(value)) {}

	void accept(const ExprVisitor& visitor) const override {
		visitor.visit(*this);
	}

	std::unique_ptr<Expr> clone() const override {
		return std::make_unique<ArraySetExpr>(name, index->clone(), value->clone());
	}
};

//	NOT USED
//	reads input from user
class InputExpr : public Expr {
public:
	InputExpr() {}

	void accept(const ExprVisitor& visitor) const override {
		visitor.visit(*this);
	}

	std::unique_ptr<Expr> clone() const override {
		return std::make_unique<InputExpr>();
	}
};

/**
 * @class CallExpr
 * @brief Represents a function call expression.
 */
class CallExpr : public Expr {
public:
	/** The function name. */
	std::unique_ptr<Expr> callee;

	/** The closing parenthesis token(used for error reporting) */
	Token paren;

	/** The arguments for the function call. */
	std::vector<std::unique_ptr<Expr>> arguments;

	CallExpr(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments)
		: callee(std::move(callee)), paren(paren), arguments(std::move(arguments)) {}

	void accept(const ExprVisitor& visitor) const override {
		visitor.visit(*this);
	}

	std::unique_ptr<Expr> clone() const override {
		std::vector<std::unique_ptr<Expr>> copiedArgs;
		for (const auto& arg : arguments) {
			copiedArgs.push_back(arg->clone());
		}
		return std::make_unique<CallExpr>(callee->clone(), paren, std::move(copiedArgs));
	}
};
