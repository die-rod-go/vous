#pragma once
#include "expr.h"
#include <sstream>
#include <string>

class ASTPrinter : public ExprVisitor {
public:
	std::string print(const Expr& expr);

	void visit(const UnaryExpr& expr) const override;
	void visit(const BinaryExpr& expr) const override;
	void visit(const GroupingExpr& expr) const override;
	void visit(const LiteralExpr& expr) const override;

	void visit(const VariableExpr& expr) const override;
	void visit(const AssignmentExpr& expr) const override;
	void visit(const ArrayPushExpr& expr) const override {};
	void visit(const ArrayAccessExpr& expr) const override {};
	void visit(const ArraySetExpr& expr) const override {};

private:
	mutable std::ostringstream result;
};
